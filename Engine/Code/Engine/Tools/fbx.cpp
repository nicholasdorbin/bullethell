#include "Engine/Tools/fbx.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Renderer/Shaders/MeshBuilder.hpp"
#include "Engine/Math/Matrix44Stack.hpp"
#include "Engine/Renderer/Skeleton.hpp"
#include "Engine/Animation/Motion.hpp"
#include "Engine/Math/UIntVector4.hpp"

#define UNREFERENCED(x) x

#if defined(TOOLS_BUILD)
#include <fbxsdk.h>
#pragma comment(lib, "libfbxsdk-md.lib")
	//For TOOLS only

typedef std::map<int, FbxNode*> IndexToNodeMap;
typedef std::pair<int, FbxNode*> IndexToNodePair;

static IndexToNodeMap s_nodeMap;


static Vector3 ToEngineVec3(FbxVector4 const &fbx_vec4)
{
	return Vector3((float)fbx_vec4.mData[0],
		(float)fbx_vec4.mData[1],
		(float)fbx_vec4.mData[2]);
}
//-----------------------------------------------------------------------------------------------
static Vector4 ToEngineVec4(FbxDouble4 const &v)
{
	return Vector4((float)v.mData[0],
		(float)v.mData[1],
		(float)v.mData[2],
		(float)v.mData[3]);
}


//-----------------------------------------------------------------------------------------------
static mat44_fl ToEngineMatrix(FbxMatrix const &fbx_mat)
{
	mat44_fl mat;
	MatrixSetRows(&mat,
		ToEngineVec4(fbx_mat.mData[0]),
		ToEngineVec4(fbx_mat.mData[1]),
		ToEngineVec4(fbx_mat.mData[2]),
		ToEngineVec4(fbx_mat.mData[3]));

	//#TODO Transpose without having other things break
	//MatrixTranspose(&mat);

	return mat;
}


static bool HasSkinWeights(FbxMesh const *mesh)
{
	int deformer_count = mesh->GetDeformerCount(FbxDeformer::eSkin);
	return (deformer_count > 0);
}


static void AddHighestWeight(skin_weight_t *skin_weight, int joint_idx, float weight)
{
	// EXERCISE TO THE READER
	// Replace the lowest value with what was sent in, IF you are higher
	// than the lower value.
	
	float* lowestWeight = &skin_weight->weights.x;
	unsigned int* lowestIndex = &skin_weight->indices.x;

	if (skin_weight->weights.y < *lowestWeight)
	{
		lowestWeight = &skin_weight->weights.y;
		lowestIndex = &skin_weight->indices.y;
	}

	if (skin_weight->weights.z < *lowestWeight)
	{
		lowestWeight = &skin_weight->weights.z;
		lowestIndex = &skin_weight->indices.z;
	}

	if (skin_weight->weights.w < *lowestWeight)
	{
		lowestWeight = &skin_weight->weights.w;
		lowestIndex = &skin_weight->indices.w;
	}

	if (weight > *lowestWeight)
	{
		*lowestWeight = weight;
		*lowestIndex = joint_idx;
	}
	
}

static void GetSkinWeights(SceneImport *import,
	std::vector<skin_weight_t> &skin_weights,
	FbxMesh const *mesh)
{
	import;
	//size_t control_point_count = mesh->GetControlPointsCount();
	for (size_t i = 0; i < skin_weights.size(); ++i) {
		skin_weights[i].indices = UIntVector4(0, 0, 0, 0);
		skin_weights[i].weights = Vector4(0.f, 0, 0, 0);
	}

	int deformer_count = mesh->GetDeformerCount(FbxDeformer::eSkin);
	ASSERT_OR_DIE(deformer_count == 1, "Deformer Count == 1");
	for (int didx = 0; didx < deformer_count; ++didx) {
		FbxSkin *skin = (FbxSkin*)mesh->GetDeformer(didx, FbxDeformer::eSkin);

		if (nullptr == skin) {
			continue;
		}

		// Clusters are a link between this skin object, bones, 
		// and the verts that bone affects.
		int cluster_count = skin->GetClusterCount();
		for (int cidx = 0; cidx < cluster_count; ++cidx) {
			FbxCluster *cluster = skin->GetCluster(cidx);
			FbxNode const *link_node = cluster->GetLink();

			// Not associated with a bone - ignore it, we
			// don't care about it. 
			if (nullptr == link_node) {
				continue;
			}
			IndexToNodeMap::iterator iter = s_nodeMap.begin();
			int joint_index = -1;
		
			for (iter = s_nodeMap.begin(); iter != s_nodeMap.end(); ++iter)
			{
				if (link_node == iter->second)
				{
					joint_index = iter->first;
				}
			}

			int joint_idx = joint_index;//import->skeletons[0]->GetJointIndexForJointName(link_node, &s_nodeMap); // you guys should have something like this
 			if (joint_idx == -1) {
				continue;
 			}

			int *control_point_indices = cluster->GetControlPointIndices();
			int index_count = cluster->GetControlPointIndicesCount();
			double *weights = cluster->GetControlPointWeights();

			for (int i = 0; i < index_count; ++i) {
				int control_idx = control_point_indices[i];
				double weight = weights[i];

				skin_weight_t *skin_weight = &skin_weights[control_idx];
				AddHighestWeight(skin_weight, joint_idx, (float)weight);
			}
		}
	}
	
	for (unsigned int i = 0; i < skin_weights.size(); ++i) {
		// Renormalize all the skin weights
		// If skin_weights were never added - make sure
		// you set it's weights to 1, 0, 0, 0
		float total_weight = skin_weights[i].weights.x + skin_weights[i].weights.y + skin_weights[i].weights.z + skin_weights[i].weights.w;
		if (total_weight > 0.0f) {
			skin_weights[i].weights = skin_weights[i].weights / total_weight;

		}
		else
		{
			skin_weights[i].weights = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
		};

	}
	
	
}

static mat44_fl GetGeometricTransform(FbxNode *node)
{
	mat44_fl ret = mat44_fl::identity;
	//MatrixMakeIdentity(&ret);

	if ((node != nullptr) && (node->GetNodeAttribute() != nullptr)) {
		FbxVector4 const geo_trans = node->GetGeometricTranslation(FbxNode::eSourcePivot);
		FbxVector4 const geo_rot = node->GetGeometricRotation(FbxNode::eSourcePivot);
		FbxVector4 const geo_scale = node->GetGeometricScaling(FbxNode::eSourcePivot);

		FbxMatrix geo_mat;
		geo_mat.SetTRS(geo_trans, geo_rot, geo_scale);

		ret = ToEngineMatrix(geo_mat);
	}

	return ret;
}

static char const* GetAttributeType(FbxNodeAttribute::EType type)
{
	switch (type) {
	case FbxNodeAttribute::eUnknown: return "unidentified";
	case FbxNodeAttribute::eNull: return "null";
	case FbxNodeAttribute::eMarker: return "marker";
	case FbxNodeAttribute::eSkeleton: return "skeleton";
	case FbxNodeAttribute::eMesh: return "mesh";
	case FbxNodeAttribute::eNurbs: return "nurbs";
	case FbxNodeAttribute::ePatch: return "patch";
	case FbxNodeAttribute::eCamera: return "camera";
	case FbxNodeAttribute::eCameraStereo: return "stereo";
	case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
	case FbxNodeAttribute::eLight: return "light";
	case FbxNodeAttribute::eOpticalReference: return "optical reference";
	case FbxNodeAttribute::eOpticalMarker: return "marker";
	case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
	case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
	case FbxNodeAttribute::eBoundary: return "boundary";
	case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
	case FbxNodeAttribute::eShape: return "shape";
	case FbxNodeAttribute::eLODGroup: return "lodgroup";
	case FbxNodeAttribute::eSubDiv: return "subdiv";
	default: return "unknown";
	}
}

static void PrintAttribute(FbxNodeAttribute *attribute, int depth)
{
	if (nullptr == attribute)
	{
		return;
	}

	FbxNodeAttribute::EType type =
		attribute->GetAttributeType();

	char const *type_name = GetAttributeType(type);
	char const *attrib_name = attribute->GetName();

	DebuggerPrintf("%*s- type='%s', name='%s'", depth, " ", type_name, attrib_name);

}

static void PrintNode(FbxNode *node, int depth)
{
	DebuggerPrintf("%*sNode [%s] \n", depth, " ", node->GetName());

	for (int i = 0; i < node->GetNodeAttributeCount(); ++i)
	{
		PrintAttribute(node->GetNodeAttributeByIndex(i), depth);
	}

	for (int32_t i = 0; i < node->GetChildCount(); ++i)
	{
		PrintNode(node->GetChild(i), depth + 1);
	}
}

void FbxListScene(std::string filename)
{
	FbxManager *fbx_manager = FbxManager::Create();
	if (nullptr == fbx_manager)
	{
		DebuggerPrintf("Could not create fbx manager.");
		return;
	}

	FbxIOSettings *io_settings =
		FbxIOSettings::Create(fbx_manager, IOSROOT);
	fbx_manager->SetIOSettings(io_settings);

	//Create importer
	FbxImporter *importer =
		FbxImporter::Create(fbx_manager, "");

	bool result = importer->Initialize(filename.c_str(), -1, fbx_manager->GetIOSettings());

	if (result)
	{
		//we have imported the FBX
		FbxScene *scene = FbxScene::Create(fbx_manager, "");

		bool import_successful = importer->Import(scene);
		if (import_successful)
		{
			FbxNode *root = scene->GetRootNode();
			PrintNode(root, 0);
		}

		FBX_SAFE_DESTROY(scene);
	}
	else
	{
		DebuggerPrintf("Could not import scene: %s", filename);
	}

	FBX_SAFE_DESTROY(importer);
	FBX_SAFE_DESTROY(io_settings);
	FBX_SAFE_DESTROY(fbx_manager);



	g_theConsole->ConsolePrint("Fbx List called");
}
/*
COMMAND(fbx_list)
{
	std::string filename = args.get_next_string();
	FbxListScene(filename);
}
*/

void FbxList(Command& args)
{

	std::string filename = args.m_argList[1];
	FbxListScene(filename);
}

#define ASSERT_RETURN(e) if(!e) {DebugBreak();return;}

static Vector3 ToVec3(FbxVector4 const &fbx_vec4)
{
	return Vector3((float)fbx_vec4.mData[0],
		(float)fbx_vec4.mData[1],
		(float)fbx_vec4.mData[2]);
}




static bool GetPosition(Vector3 *outPosition,
	mat44_fl const &transform,
	FbxMesh *mesh,
	int polyIndex,
	int vertIndex)
{
	FbxVector4 fbx_pos;
	int control_index = mesh->GetPolygonVertex(polyIndex, vertIndex);

	fbx_pos = mesh->GetControlPointAt(control_index);
	//#TODO Fix for our Row matrixes, not collumns
	//*outPosition = transform * Vector4(ToVec3(fbx_pos),1.f).GetXYZ();
	*outPosition = ( Vector4(ToEngineVec3(fbx_pos), 1.0f) * transform).GetXYZ();
	return true;
}

template <typename ElemType, typename VarType>
static bool GetObjectFromElement(FbxMesh *mesh,
	int poly_idx,
	int vert_idx,
	ElemType *elem,
	VarType *out_var)
{
	if (nullptr == elem) {
		return false;
	}

	switch (elem->GetMappingMode()) {
	case FbxGeometryElement::eByControlPoint:
	{
		int control_idx = mesh->GetPolygonVertex(poly_idx, vert_idx);
		switch (elem->GetReferenceMode()) {
		case FbxGeometryElement::eDirect:
			if (control_idx < elem->GetDirectArray().GetCount()) {
				*out_var = elem->GetDirectArray().GetAt(control_idx);
				return true;
			}
			break;

		case FbxGeometryElement::eIndexToDirect:
			if (control_idx < elem->GetIndexArray().GetCount()) {
				int index = elem->GetIndexArray().GetAt(control_idx);
				*out_var = elem->GetDirectArray().GetAt(index);
				return true;
			}
			break;

		default:
			break;
		}
	} break;

	case FbxGeometryElement::eByPolygonVertex:
	{
		int direct_vert_idx = (poly_idx * 3) + vert_idx;
		switch (elem->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			if (direct_vert_idx < elem->GetDirectArray().GetCount()) {
				*out_var = elem->GetDirectArray().GetAt(direct_vert_idx);
				return true;
			}
			break;

		case FbxGeometryElement::eIndexToDirect:
			if (direct_vert_idx < elem->GetIndexArray().GetCount()) {
				int index = elem->GetIndexArray().GetAt(direct_vert_idx);
				*out_var = elem->GetDirectArray().GetAt(index);
				return true;
			}
			break;

		default:
			break;
		}
	} break;

	default:
		//ASSERT_OR_DIE("Undefined mapping mode.");
		break;
	}

	return false;
}


static bool GetColor(Vector4 *out_color,
	mat44_fl const &transform,
	FbxMesh *mesh,
	int poly_idx,
	int vert_idx)
{
	transform;

	FbxColor color;
	FbxGeometryElementVertexColor *colors = mesh->GetElementVertexColor(0);
	if (GetObjectFromElement(mesh, poly_idx, vert_idx, colors, &color)) {
		Vector4 vec4color = Vector4((float)color.mRed, (float)color.mGreen, (float)color.mBlue, (float)color.mAlpha);
		*out_color = vec4color;
		return true;
	}

	return false;
}

static bool GetNormal(Vector3 *out_normal,
	mat44_fl const &transform,
	FbxMesh *mesh,
	int poly_idx,
	int vert_idx)
{
	FbxVector4 normal;
	FbxGeometryElementNormal *uvs = mesh->GetElementNormal(0);
	if (GetObjectFromElement(mesh, poly_idx, vert_idx, uvs, &normal)) {
		Vector3 n = ToVec3(normal);
		*out_normal = ( Vector4(n, 0.0f) * transform).GetXYZ();
		return true;
	}

	return false;
}

static bool GetUV(Vector2 *out_uv,
	FbxMesh *mesh,
	int poly_idx,
	int vert_idx,
	int uv_idx)
{
	FbxVector2 uv;
	FbxGeometryElementUV *uvs = mesh->GetElementUV(uv_idx);
	if (GetObjectFromElement(mesh, poly_idx, vert_idx, uvs, &uv))
	{
		*out_uv = Vector2((float)uv.mData[0], (float)uv.mData[1]);
		return true;
	}
	return false;
}


static void ImportVertex(MeshBuilder *mb,
	mat44_fl const &transform,
	FbxMesh *mesh,
	int polyIndex,
	int vertIndex
	, std::vector<skin_weight_t> &skin_weights)
{

	Vector3 normal;
	if (GetNormal(&normal, transform, mesh, polyIndex, vertIndex))
	{
		mb->SetNormal(normal);
		//#TODO: Remove once we have a vertex that supports only normals
		Vector3 bitangent = Vector3(0.f, 0.f, 1.f);
		if (normal == bitangent || normal == -bitangent)//Need to check if colinear
		{
			bitangent = Vector3(0.f, 1.f, 0.f);
		}
		Vector3 tangent = CrossProduct(bitangent, normal);
		bitangent = CrossProduct(normal, tangent);

		mb->SetTangent(tangent);
		mb->SetBitangent(bitangent);
	}

	Vector4 color;
	if (GetColor(&color, transform, mesh, polyIndex, vertIndex)) {
		unsigned char red = (unsigned char)RangeMap(color.x, 0.0f, 1.0f, 0.0f, 255.0f);
		unsigned char green = (unsigned char)RangeMap(color.y, 0.0f, 1.0f, 0.0f, 255.0f);
		unsigned char blue = (unsigned char)RangeMap(color.z, 0.0f, 1.0f, 0.0f, 255.0f);
		unsigned char alpha = (unsigned char)RangeMap(color.w, 0.0f, 1.0f, 0.0f, 255.0f);
		mb->SetColor(Rgba(red, green, blue, alpha));
	}


	Vector2 uv;

	if (GetUV(&uv, mesh, polyIndex, vertIndex, 0))
	{
		mb->SetUV0(uv);
	}

	// Set Skin Weight
	int control_idx = mesh->GetPolygonVertex(polyIndex, vertIndex);
	if (control_idx < (int)skin_weights.size()) {
		mb->RenormalizeSkinWeights(); // just to be safe
		mb->SetBoneWeights(skin_weights[control_idx].indices, skin_weights[control_idx].weights);
	}
	else {
		mb->ClearBoneWeights();
	}

	Vector3 position;
	if (GetPosition(&position,transform, mesh, polyIndex, vertIndex))
	{
		mb->AddVertex(position);
	}
}

static void ImportMesh(SceneImport *import,
	FbxMesh *mesh,
	Matrix4x4Stack &mat_stack)
{
	MeshBuilder *mb = new MeshBuilder();

	ASSERT_RETURN(mesh->IsTriangleMesh());
	
	
	mat44_fl geo_transform = GetGeometricTransform(mesh->GetNode());
	mat_stack.Push(geo_transform);

	mat44_fl transform = mat_stack.GetTop();

	int control_point_count = mesh->GetControlPointsCount();

	// NEW STUFF IS HERE!  Before we import any vertices
	// figure out our weights for all vertices
	std::vector<skin_weight_t> skin_weights;
	skin_weights.resize(control_point_count);
	if (HasSkinWeights(mesh)) {
		GetSkinWeights(import, skin_weights, mesh);
	}
	else {
		//FbxNode *node = mesh->GetNode();

		// walk tree up till node associated with joint. 

		// Find first parent node that has a joint associated with it
		// all vertices (fully weighted)
		// all skin_weights = indices{ joint_idx, 0, 0, 0 }  weights{ 1.0f, 0.0f, 0.0f, 0.0f };
	}

	int poly_count = mesh->GetPolygonCount();
	for (int poly_idx = 0; poly_idx < poly_count; ++poly_idx) {
		int vert_count = mesh->GetPolygonSize(poly_idx);
		//ASSERT_OR_DIE(vert_count == 3);
		for (int vert_idx = 0; vert_idx < vert_count; ++vert_idx) {
			ImportVertex(mb, transform, mesh, poly_idx, vert_idx, skin_weights);
		}
	}

	mb->End();
	mat_stack.Pop();
	//Import the mesh

	import->meshes.push_back(mb);
}

static void ImportNodeAttribute(SceneImport *import,
	FbxNodeAttribute *attrib,
	Matrix4x4Stack &mat_stack)
{
	if (attrib == nullptr)
	{
		return;
	}

	switch (attrib->GetAttributeType())
	{
	case FbxNodeAttribute::eMesh:
	{
		ImportMesh(import, (FbxMesh*)attrib, mat_stack);
		break;
	}
	default:
		break;
		
	}

}

static mat44_fl GetNodeTransform(FbxNode *node)
{
	FbxMatrix fbx_local_matrix = node->EvaluateLocalTransform();
	return ToEngineMatrix(fbx_local_matrix);
	//#TODO Write this function
//	return mat44_fl();

}

static void ImportSceneNode(SceneImport *import,
	FbxNode *node,
	Matrix4x4Stack &mat_stack)
{
	if (nullptr == node)
	{
		return;
	}

	mat44_fl node_local_transform = GetNodeTransform(node);
	mat_stack.Push(node_local_transform);

	//Load Mesh
	int attrib_count = node->GetNodeAttributeCount();
	for (int attrib_index = 0; attrib_index < attrib_count; ++attrib_index)
	{
		ImportNodeAttribute(import, node->GetNodeAttributeByIndex(attrib_index), mat_stack);
	}

	//Import Children
	int childCount = node->GetChildCount();
	for (int childIndex = 0; childIndex < childCount; ++childIndex)
	{
		ImportSceneNode(import, node->GetChild(childIndex), mat_stack);
	}

	mat_stack.Pop();
}

static void TriangulateScene(FbxScene *scene)
{
	FbxGeometryConverter converter(scene->GetFbxManager());
	converter.Triangulate(scene, true/*replaces nodes with a traigulated mesh*/);
}

static Skeleton* ImportJoint(SceneImport *import,
	Matrix4x4Stack &mat_stack,
	Skeleton *cur_skeleton,
	int parent_joint_idx,
	FbxSkeleton *fbx_skeleton)
{
	Skeleton *ret_skeleton = nullptr;

	if (fbx_skeleton->IsSkeletonRoot()) {
		// NEW SKELETON!
		ret_skeleton = new Skeleton();
		import->skeletons.push_back(ret_skeleton);
	}
	else {
		ret_skeleton = cur_skeleton;
		ASSERT_OR_DIE(ret_skeleton != nullptr, nullptr);
	}

	// Same as we did for meshes.  Usually this is the identity, but some 
	// modeling programs use it.
	mat44_fl geo_transform = GetGeometricTransform(fbx_skeleton->GetNode());//GetNodeTransform(fbx_skeleton->GetNode());
	mat_stack.Push(geo_transform);

	mat44_fl model_matrix = mat_stack.GetTop();
	ret_skeleton->AddJoint(fbx_skeleton->GetNode()->GetName(),
		parent_joint_idx,
		model_matrix);

	mat_stack.Pop();
	return ret_skeleton;
}

static void ImportSkeletons(SceneImport *import,
	FbxNode *node,
	Matrix4x4Stack &matrix_stack,
	Skeleton *cur_skeleton, 	// Skeleton I'm adding to
	int parent_joint_idx,
	IndexToNodeMap &map)		// Last Joint Added
{
	// Most of this will look familiar
	if (nullptr == node) {
		return;
	}

	mat44_fl mat = GetNodeTransform(node);
	matrix_stack.Push(mat);

	int attrib_count = node->GetNodeAttributeCount();
	for (int attrib_idx = 0; attrib_idx < attrib_count; ++attrib_idx) {
		FbxNodeAttribute *attrib = node->GetNodeAttributeByIndex(attrib_idx);
		if ((attrib != nullptr)
			&& (attrib->GetAttributeType() == FbxNodeAttribute::eSkeleton)) {

			Skeleton *new_skeleton = ImportJoint(import,
				matrix_stack,
				cur_skeleton,
				parent_joint_idx,
				(FbxSkeleton*)attrib);

			
			IndexToNodePair pair (new_skeleton->m_joints.size() - 1, attrib->GetNode());
			map.insert(pair);
			// new_skeleton will either be the same skeleton
			// passed in, no skeleton, or a new skeleton
			// in the case of a new skeleton or same skeleton
			// it will become the skeleton we pass to all our
			// children.
			if (nullptr != new_skeleton) {
				cur_skeleton = new_skeleton;
				parent_joint_idx = cur_skeleton->GetLastAddedIndex();
			}
		}
	}

	// Go down the tree!
	int child_count = node->GetChildCount();
	for (int child_idx = 0; child_idx < child_count; ++child_idx) {
		ImportSkeletons(import,
			node->GetChild(child_idx),
			matrix_stack,
			cur_skeleton,
			parent_joint_idx,
			map);
		if (cur_skeleton != nullptr)
		{
			IndexToNodePair childPair(cur_skeleton->m_joints.size() - 1, node->GetChild(child_idx));
			map.insert(childPair);
		}
		
	}

	matrix_stack.Pop();
}


static mat44_fl GetNodeWorldTransformAtTime(FbxNode *node, FbxTime time, mat44_fl const &import_transform)
{
	if (node == nullptr) {
		return mat44_fl::identity;
	}

	FbxMatrix fbx_mat = node->EvaluateGlobalTransform(time);
	mat44_fl engine_matrix = ToEngineMatrix(fbx_mat);
	return  import_transform * engine_matrix;
}

void ImportMotions(SceneImport *import,
	mat44_fl &import_transform,
	IndexToNodeMap &map,
	FbxScene *scene,
	float framerate)
{
	 	// Get number of animations
	 	int animation_count = scene->GetSrcObjectCount<FbxAnimStack>();
	 	if ( 0 == animation_count ) {
	 		return;
	 	}
	 
	 	if ( 0 == import->skeletons.size() ) {
	 		return;
	 	}
	 
	 	// Timing information for animation in this scene
	 	FbxGlobalSettings &settings = scene->GetGlobalSettings();
	 	FbxTime::EMode time_mode = settings.GetTimeMode();
	 	double scene_framerate;
	 	if ( time_mode == FbxTime::eCustom ) {
	 		scene_framerate = settings.GetCustomFrameRate();
	 	}
	 	else {
	 		scene_framerate = FbxTime::GetFrameRate( time_mode );
	 	}
	 
	 	// Only supporting one skeleton for now - update when needed.
	 	uint32_t skeleton_count = import->skeletons.size();
	 	Skeleton *skeleton = import->skeletons[ 0 ];
	 	ASSERT_OR_DIE( skeleton_count == 1, "skeleton_count != 1" );
	 
	 	// Time between frames
	 	FbxTime advance;
	 	advance.SetSecondDouble( ( double ) ( 1.0f / framerate ) );
	 
	 	for ( int anim_index = 0; anim_index < animation_count; ++anim_index ) {
	 		// Import Motions
	 		FbxAnimStack *anim = scene->GetSrcObject<FbxAnimStack>();
	 		if ( nullptr == anim ) {
	 			continue;
	 		}
	 
	 		// Get duration of this specific animation.
	 		FbxTime start_time = anim->LocalStart;
	 		FbxTime end_time = anim->LocalStop;
	 		//FbxTime duration = end_time - start_time;
			FbxTime duration = end_time - 0;
	 
	 		scene->SetCurrentAnimationStack( anim );
	 
			char const *motion_name = anim->GetName();
	 		float time_span = (float)duration.GetSecondDouble();
	 
	 		Motion *motion = new Motion( motion_name, time_span, framerate, skeleton );
	 
	 		int joint_count = skeleton->GetJointCount();
	 		for ( int joint_idx = 0; joint_idx < joint_count; ++joint_idx ) {
	 			FbxNode *node = map.at(joint_idx); // map<FbxNode*, Joint*> &map, 
	 
	 											  // Extracting World Position
	 											  // Local, you would need to grab parent as well
	 			mat44_fl *bone_keyframes = motion->GetJointKeyFrames( joint_idx );
	 
	 			FbxTime eval_time = FbxTime( 0 );
	 			for ( uint32_t frame_idx = 0; frame_idx < (unsigned int) motion->m_frameCount; ++frame_idx ) {
					mat44_fl *bone_keyframe = bone_keyframes + frame_idx;
					mat44_fl bone_transform = GetNodeWorldTransformAtTime( node, eval_time, import_transform );
	 				*bone_keyframe = bone_transform;
	 
	 				eval_time += advance;
	 			}
	 		}
	 
	 		import->motions.push_back( motion );
	 	}
}

static void ImportScene(SceneImport *import, FbxScene *scene, Matrix4x4Stack &mat_stack)
{
	TriangulateScene(scene);

	IndexToNodeMap index_to_node;
	FbxNode *node = scene->GetRootNode();
	ImportSkeletons(import, node, mat_stack, nullptr, -1, index_to_node);
	s_nodeMap = index_to_node;
	ImportSceneNode(import, node, mat_stack);
	// Top contains just our change of basis and scale matrices at this point
	ImportMotions( import, mat_stack.GetTop(), index_to_node, scene, 30.f );
}


static mat44_fl GetSceneBasis(FbxScene* scene)
{
	fbxsdk::FbxAxisSystem axis_system = scene->GetGlobalSettings().GetAxisSystem();
	FbxAMatrix mat;
	axis_system.GetMatrix(mat);

	return ToEngineMatrix(mat);
}

SceneImport* FbxLoadSceneFromFile(std::string filename,
	mat44_fl engine_basis,
	bool is_engine_basis_right_handed,
	mat44_fl transform)
{
	g_theConsole->ConsolePrint("Fbx Load called");
	/*
	FbxScene *scene = //Get the scene

	SceneImport *import = new SceneImport();
	Matrix4x4Stack mat_stack;
	ImportScene(import,scene, matrix_stack);

	//Do normal kind of cleanup

	return import;
	*/
	FbxManager *fbx_manager = FbxManager::Create();
	if (nullptr == fbx_manager)
	{
		DebuggerPrintf("Could not create fbx manager.");
		return nullptr;
	}

	FbxIOSettings *io_settings =
		FbxIOSettings::Create(fbx_manager, IOSROOT);
	fbx_manager->SetIOSettings(io_settings);

	//Create importer
	FbxImporter *importer =
		FbxImporter::Create(fbx_manager, "");

	bool result = importer->Initialize(filename.c_str(), -1, fbx_manager->GetIOSettings());

	if (result)
	{
		//we have imported the FBX
		FbxScene *scene = FbxScene::Create(fbx_manager, "");

		bool import_successful = importer->Import(scene);
		if (import_successful)
		{
			//FbxNode *root = scene->GetRootNode();
			SceneImport *import = new SceneImport();
			Matrix4x4Stack mat_stack;

			mat_stack.Push(transform);
			mat_stack.Push(engine_basis);

			//Set up initial transforms
			mat44_fl scene_basis = GetSceneBasis(scene);
			MatrixTranspose(&scene_basis);

			if (!is_engine_basis_right_handed)
			{
				Vector3 forward = MatrixGetForward(&scene_basis);
				MatrixSetForward(&scene_basis, -forward);

			}
			mat_stack.Push(scene_basis);

			ImportScene(import, scene, mat_stack);

			return import;
		}

		FBX_SAFE_DESTROY(scene);
	}
	else
	{
		DebuggerPrintf("Could not import scene: %s", filename);
	}

	FBX_SAFE_DESTROY(importer);
	FBX_SAFE_DESTROY(io_settings);
	FBX_SAFE_DESTROY(fbx_manager);

	return nullptr;

	
}
#else //!defined (TOOLS_BUILD)
void FbxListScene(std::string filename) {}

SceneImport* FbxLoadSceneFromFile(std::string filename) { return nullptr; }
SceneImport* FbxLoadSceneFromFile(std::string filename,
	mat44_fl engine_basis,
	bool is_engine_basis_right_handed,
	mat44_fl transform) {
	filename;
	engine_basis;
	is_engine_basis_right_handed;
	transform;
	return nullptr;}

#endif