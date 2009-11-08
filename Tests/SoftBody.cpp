
//Thanks to go/going
//http://www.bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=9&t=2867&p=11432
//http://irrlicht.sourceforge.net/phpBB2/viewtopic.php?t=31091
//http://sssiii.seesaa.net/article/109665032.html (Japanese)	
#include <iostream> 
#include <map> 
#include <vector> 
#include <list> 

#include <btBulletDynamicsCommon.h> 
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h" 
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h" 
#include "BulletSoftBody/btSoftBodyHelpers.h" 
#include "BulletSoftBody/btSoftBody.h" 

#include "BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.h" 
#include "BulletCollision/NarrowPhaseCollision/btGjkEpa2.h" 
#include "LinearMath/btQuickprof.h" 
#include "LinearMath/btIDebugDraw.h" 
#include "LinearMath/btConvexHull.h" 

#include <irrlicht.h> 


using namespace irr; 
using namespace core; 
using namespace scene; 
using namespace video; 
using namespace io; 
using namespace gui; 

// Vertices 
std::map<int, btSoftBody::Node*> vertices; 
// Indices 
std::vector<int> indices; 
// Node->int（index）の一時マップを作成 
// Temporary placeholder for nodes 
std::map<btSoftBody::Node*, int> node_map; 

#define CUBE_HALF_EXTENTS 15 

struct MeshData 
{ 
    btScalar *mqo_vertices; 
    int *mqo_indices; 
    int indexCount, vertexCount; 
}; 

MeshData testMesh; 

int main (void) 
{ 
    // Irrlichtの準備 
    IrrlichtDevice *device = createDevice(EDT_OPENGL, dimension2d<u32>(640, 480), 16, 
        false, true, false, 0); 
    device->setWindowCaption(L"Irrlicht + Bullet : SoftBody Demo"); 
    IVideoDriver* driver = device->getVideoDriver(); 
    ISceneManager *smgr = device->getSceneManager(); 

    // カメラの設定 
    ICameraSceneNode *camera = smgr->addCameraSceneNodeFPS(0, 150, 0.5, -1, 0, 0, false); 
    camera->setPosition(core::vector3df(0,400,-300)); 
    camera->setFarValue(10000); 
    camera->setTarget(core::vector3df(0, 300, 0)); 

    // SoftBodyのメッシュデータをファイルから読み込み（Irrlicht用） 

    // you shoud change this object 
    IAnimatedMesh *cubeMesh = smgr->getMesh("../media/test.3ds"); 
    IAnimatedMeshSceneNode *cubeNode = smgr->addAnimatedMeshSceneNode(cubeMesh, 0, -1, vector3df(0, 0, 0), vector3df(0,0,0), vector3df(1,1,1), false); 
	cubeNode->setMaterialTexture(0, driver->getTexture("../media/earth.jpg"));
    cubeNode->setMaterialFlag(video::EMF_LIGHTING, false); 

    // 地面の作成（Irrlicht用） 
    IAnimatedMesh *planemesh = smgr->addHillPlaneMesh("myHill", dimension2d<f32>(24, 24), dimension2d<u32>(100, 100)); 
    ISceneNode *q3sn = smgr->addOctTreeSceneNode(planemesh); 
    q3sn->setMaterialFlag(video::EMF_LIGHTING, false); 

   // you should change this resource 
    q3sn->setMaterialTexture(0, driver->getTexture("../media/wall.jpg")); 

    // SoftBody用ワールド情報 
    btSoftBodyWorldInfo   m_softBodyWorldInfo; 

    // ワールドの広さ 
    btVector3 worldAabbMin(-10000,-10000,-10000); 
    btVector3 worldAabbMax(10000,10000,10000); 
    // プロキシの最大数（衝突物体のようなもの） 
    int maxProxies = 1024; 
    // broadphaseの作成（SAP法） 
    btAxisSweep3* broadphase = new btAxisSweep3(worldAabbMin,worldAabbMax,maxProxies); 
    m_softBodyWorldInfo.m_broadphase = broadphase; 

    // デフォルトの衝突設定とディスパッチャの作成 
    //    const btDefaultCollisionConfigurationInfo collisionConfigInfo; 
    btDefaultCollisionConfiguration* collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration(); 
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration); 
    m_softBodyWorldInfo.m_dispatcher = dispatcher; 

    // 衝突解決ソルバ 
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver; 

    // Soft-Rigit動的世界の作成 
    btSoftRigidDynamicsWorld* dynamicsWorld = new btSoftRigidDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration); 

    // 地面の衝突形状の作成 
    btCollisionShape* groundShape = new btBoxShape (btVector3(2000,CUBE_HALF_EXTENTS,2000)); 
    // 地面のMotionStateの設定 
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-CUBE_HALF_EXTENTS/2.0,0))); 
    // 地面の初期情報を設定 
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0)); 
    // 地面の剛体の作成 
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI); 
    groundRigidBody->setCollisionFlags( groundRigidBody->getCollisionFlags() |  btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK ); 
    // ワールドに地面の剛体を追加 
    dynamicsWorld->addRigidBody(groundRigidBody); 

    // 重力の設定 
    dynamicsWorld->setGravity(btVector3(0,-10.0,0)); 
    // softBodyWorldInfoの初期化・設定 
    m_softBodyWorldInfo.m_sparsesdf.Initialize(); 
    m_softBodyWorldInfo.m_gravity.setValue(0,-10.0,0); 
    m_softBodyWorldInfo.air_density      =   (btScalar)1.2; 
    m_softBodyWorldInfo.water_density   =   0; 
    m_softBodyWorldInfo.water_offset   =   0; 
    m_softBodyWorldInfo.water_normal   =   btVector3(0,0,0); 

    int cMeshBuffer, j; 
    IMeshBuffer *mb; 
    video::S3DVertex* mb_vertices; 
    u16* mb_indices; 

    // getMeshで取得したメッシュ情報の取得 
    for (cMeshBuffer=0; cMeshBuffer<cubeMesh->getMesh(0)->getMeshBufferCount(); cMeshBuffer++) 
    { 
        // メッシュバッファの取得 
        mb = cubeMesh->getMesh(0)->getMeshBuffer(cMeshBuffer); 

        // 頂点情報のポインタを取得。この値を書き換えれば頂点座標を変更できる。 
        mb_vertices = (irr::video::S3DVertex*)mb->getVertices(); 
        // 面のインデックスのポインタを取得。この値を書き換えれば面のインデックスを変更できる。 
        mb_indices  = mb->getIndices(); 
    } 

    // 読み込んだメッシュデータの最適化 
    // 3dsファイル（.objも？）をIrrlichtで読み込むと、（Index数＝Vertex数）となり、Indexが一意で、Vertexが重複する。 
    // 大量の頂点からSoftBodyを作成しようとするとものすごく時間がかかるので、Vertexの重複をなくす。 
    // そのためにまず、重複のないIndexと重複のあるIndexのマップを作成 
    std::map<int, int> index_map; 
    std::map<int, int> bullet_map; 
    std::map<int, S3DVertex> vertex_map; 
    int count = 0; 
    for (int i=0; i<mb->getIndexCount(); i++) 
    { 
        int iIndex = mb_indices[i]; 
        vector3df iVertex = mb_vertices[iIndex].Pos; 
        bool isFirst = true; 
        for (int j=0; j<i; j++) 
        { 
            int jIndex = mb_indices[j]; 
            vector3df jVertex = mb_vertices[jIndex].Pos; 
            if (iVertex == jVertex) 
            { 
                index_map.insert(std::make_pair(i, j)); 
                isFirst = false; 
                break; 
            } 
        } 
        // 初めての場合はBullet用のIndexと頂点を作成 
        if (isFirst) 
        { 
            // IrrlichtのIndex→重複のあるIndex 
            index_map.insert(std::make_pair(i, i)); 
            // とびとびのIndex→連続のIndex 
            bullet_map.insert(std::make_pair(i, count)); 
            // 連続Indexと頂点情報のマップ 
            vertex_map.insert(std::make_pair(count, mb_vertices[iIndex])); 
            count++; 
        } 
    } 

    // IrrlichtとBulletの間の一時的なIndex-Vertex情報の作成 
    testMesh.indexCount = mb->getIndexCount(); 
    testMesh.mqo_indices = new int[testMesh.indexCount]; 
    testMesh.vertexCount = vertex_map.size(); 
    testMesh.mqo_vertices = new btScalar[testMesh.vertexCount*3]; 

    std::cout << "IndexCount=" << testMesh.indexCount << ", VertexCount=" << testMesh.vertexCount << std::endl; 
    // メッシュからポリゴン情報を取得し配列に格納 
    for (j=0; j<mb->getIndexCount(); j++) 
    { 
        // 重複のないIndex→重複のあるとびとびのIndex 
        int index1 = index_map.find(j)->second; 
        // 重複のあるとびとびIndex→重複のある順番のIndex 
        int index2 = bullet_map.find(index1)->second; 
        testMesh.mqo_indices[j]   = index2; 
    } 
    // SoftBodyを作成するための頂点座標を格納 
    for (j=0; j<testMesh.vertexCount; j++) 
    { 
        testMesh.mqo_vertices[3*j] =   vertex_map[j].Pos.X; 
        testMesh.mqo_vertices[3*j+1] = vertex_map[j].Pos.Y; 
        testMesh.mqo_vertices[3*j+2] = -vertex_map[j].Pos.Z; 
    } 

    std::cout << "create softbody" << std::endl; 
    // 三角メッシュからSoftBodyを作成（btSoftBodyHelpersを使用） 
    btSoftBody* cubeSoftBody = btSoftBodyHelpers::CreateFromTriMesh( 
        m_softBodyWorldInfo, testMesh.mqo_vertices, testMesh.mqo_indices, testMesh.indexCount/3); 

    std::cout << "create map" << std::endl; 
    // 作成されたSoftBodyの面情報をインデックスと関連付ける（node->index） 
    for (int i=0; i<cubeSoftBody->m_faces.size(); i++) 
    { 
        btSoftBody::Face face = cubeSoftBody->m_faces[i]; 

        for (int j=0; j<3; j++) 
        { 
            if (node_map.find(face.m_n[j]) == node_map.end()) 
            { 
                node_map.insert(std::make_pair(face.m_n[j], node_map.size())); 
            } 
        } 

        for (int j=0; j<3; j++) 
        { 
            // Bulletのインデックス 
            indices.push_back(node_map.find(face.m_n[j])->second); 
        } 
    } 
    // Reverse node->index to index->node (should be unique on both sides) 
    std::map<btSoftBody::Node*, int>::const_iterator node_iter; 
    for (node_iter = node_map.begin(); node_iter != node_map.end(); ++node_iter) 
    { 
        // Bulletの頂点 
        vertices.insert(std::make_pair(node_iter->second, node_iter->first)); 
    } 

    // IrrlichtのVertexとBulletのVertexを関連付ける必要がある 
    std::cout << "update Irrlicht vertices" << std::endl; 
    // 関連付けるためのマップ 
    std::map<int, int> testMesh_map; 
    // SoftBodyのIndex→Vertexのイテレータ 
    std::map<int, btSoftBody::Node*>::const_iterator it; 

    // 頂点の数だけ関連付け 
    for (int i=0; i<mb->getVertexCount(); i++) 
    { 
        for (it=vertices.begin(); it != vertices.end(); ++it) 
        { 
            // 同じ座標を指すIrrlichtの頂点番号とBulletの頂点番号を関連付け 
            int v_index = it->first; 
            btSoftBody::Node* node = it->second; 
            if (node->m_x.x() ==  mb_vertices[i].Pos.X && 
                node->m_x.y() ==  mb_vertices[i].Pos.Y && 
                node->m_x.z() == -mb_vertices[i].Pos.Z) 
            { 
                // IrrlichtとBulletのマップを作成 
                testMesh_map.insert(std::make_pair(i, v_index)); 
                break; 
            } 
        } 
    } 

    // SoftBodyのパラメータ設定 
    std::cout << "addSoftBody" << std::endl; 
    cubeSoftBody->m_cfg.kDP = 0.0;// Damping coefficient [0,1] 
    cubeSoftBody->m_cfg.kDF = 0.2;// Dynamic friction coefficient [0,1] 
    cubeSoftBody->m_cfg.kMT = 0.01;// Pose matching coefficient [0,1] 
    cubeSoftBody->m_cfg.kCHR = 1.0;// Rigid contacts hardness [0,1] 
    cubeSoftBody->m_cfg.kKHR = 0.8;// Kinetic contacts hardness [0,1] 
    cubeSoftBody->m_cfg.kSHR = 1.0;// Soft contacts hardness [0,1] 
    cubeSoftBody->m_cfg.piterations=2; 
    //cubeSoftBody->m_materials[0]->m_kLST = 0.8; 
	cubeSoftBody->m_materials[0]->m_kLST = 0.02; 
    cubeSoftBody->m_materials[0]->m_kAST = 0.8; 
    cubeSoftBody->m_materials[0]->m_kVST = 0.8; 
    cubeSoftBody->scale(btVector3(10,10,10)); 
    cubeSoftBody->setPose(true,false);//(true, true); 
    cubeSoftBody->generateBendingConstraints(2); 
    cubeSoftBody->randomizeConstraints(); 

    // 初期位置設定、BulletへのSoftBodyの追加 
    btMatrix3x3 m; 
    m.setIdentity(); 
    cubeSoftBody->transform(btTransform(m,btVector3(0, 400, 0))); 
    dynamicsWorld->addSoftBody(cubeSoftBody); 

    std::cout << "start simulation" << std::endl; 
    // シミュレーションステップ 
    while(device->run()) 
    { 
        // シミュレーションを進める。間隔は60Hz 
        dynamicsWorld->stepSimulation(1/60.0f, 1); 
		CProfileManager::dumpAll();

        // Irrlichtの頂点情報の更新 
        for (int i=0; i<mb->getVertexCount(); i++) 
        { 
            // Irrlictの頂点番号→Bulletの頂点番号 
            int index = testMesh_map.find(i)->second; 
            // Bulletの頂点番号からノードを取得し,頂点座標を更新 
            btSoftBody::Node* node = vertices.find(index)->second; 
            // Irrlicht側はmb_vertices[i]でOK 
            mb_vertices[i].Pos.X = node->m_x.x(); 
            mb_vertices[i].Pos.Y = node->m_x.y(); 
            mb_vertices[i].Pos.Z = -node->m_x.z(); 
        } 

        if (GetAsyncKeyState(VK_SPACE)) 
        { 
            // 0番目のノードに力を加える 
            cubeSoftBody->addForce(btVector3(0, 10, 0), 0); 
        } 
        else if (GetAsyncKeyState(VK_ESCAPE)) 
        { 
            break; 
        } 

        driver->beginScene(true, true, SColor(0,200,200,200)); 
        smgr->drawAll(); 
        driver->endScene(); 
    } 
    device->drop(); 

    /* Clean up   */ 
    for(int i=dynamicsWorld->getNumCollisionObjects()-1;i>0;i--) 
    { 
        btCollisionObject*   obj=dynamicsWorld->getCollisionObjectArray()[i]; 
        btRigidBody*      body=btRigidBody::upcast(obj); 
        if(body&&body->getMotionState()) 
        { 
            delete body->getMotionState(); 
        } 
        while(dynamicsWorld->getNumConstraints()) 
        { 
            btTypedConstraint*   pc=dynamicsWorld->getConstraint(0); 
            dynamicsWorld->removeConstraint(pc); 
            delete pc; 
        } 
        btSoftBody* softBody = btSoftBody::upcast(obj); 
        if (softBody) 
        { 
            dynamicsWorld->removeSoftBody(softBody); 
        } else 
        { 
            dynamicsWorld->removeCollisionObject(obj); 
        } 
        delete obj; 
    } 

    delete [] testMesh.mqo_indices; 
    delete [] testMesh.mqo_vertices; 

    // ワールド・ソルバ・その他もろもろの削除 
    delete dynamicsWorld; 
    delete solver; 
    delete collisionConfiguration; 
    delete dispatcher; 
    delete broadphase; 

    return 0; 
} 