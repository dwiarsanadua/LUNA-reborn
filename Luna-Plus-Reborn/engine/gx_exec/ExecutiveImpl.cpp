#include "ExecutiveImpl.h"
#include <spdlog/spdlog.h>

ExecutiveImpl::ExecutiveImpl()
    : m_refCount(1), m_renderDevice(nullptr), m_textureManager(nullptr), m_shaderManager(nullptr)
    , m_dwViewportIndex(0), m_dwRenderMode(0), m_dwPickingMode(0)
    , m_dwFrameCount(0), m_dwGameFPS(60), m_dwTicksPerFrame(16) {}

ExecutiveImpl::~ExecutiveImpl() {}

STDMETHODIMP ExecutiveImpl::QueryInterface(REFIID, PPVOID) { return S_OK; }
STDMETHODIMP_(ULONG) ExecutiveImpl::AddRef() { return ++m_refCount; }
STDMETHODIMP_(ULONG) ExecutiveImpl::Release() {
    ULONG r = --m_refCount;
    if (r == 0) delete this;
    return r;
}

I4DyuchiGXRenderer* __stdcall ExecutiveImpl::GetRenderer() { return nullptr; }
I4DyuchiGXGeometry* __stdcall ExecutiveImpl::GetGeometry() { return nullptr; }
HRESULT __stdcall ExecutiveImpl::GetRenderer(I4DyuchiGXRenderer**) { return E_NOTIMPL; }
HRESULT __stdcall ExecutiveImpl::GetGeometry(I4DyuchiGXGeometry**) { return E_NOTIMPL; }
HRESULT __stdcall ExecutiveImpl::GetFileStorage(I4DyuchiFileStorage**) { return E_NOTIMPL; }

BOOL __stdcall ExecutiveImpl::InitializeWithoutRegistry(char*, char*, HWND, DISPLAY_INFO* pInfo, DWORD, DWORD, DWORD, DWORD, DWORD, ErrorHandleProc)
{
    if (!pInfo) return FALSE;
    m_renderDevice = std::make_unique<RenderDevice>();
    RenderDeviceConfig cfg;
    cfg.width = pInfo->dwWidth;
    cfg.height = pInfo->dwHeight;
    if (!m_renderDevice->Init(cfg)) {
        spdlog::error("ExecutiveImpl: RenderDevice init failed");
        return FALSE;
    }
    m_textureManager = std::make_unique<TextureManager>();
    m_shaderManager = std::make_unique<ShaderManager>();
    spdlog::info("ExecutiveImpl: initialized ({}x{})", pInfo->dwWidth, pInfo->dwHeight);
    return TRUE;
}

BOOL __stdcall ExecutiveImpl::Initialize(HWND, DISPLAY_INFO*, DWORD, DWORD, DWORD, DWORD, DWORD, ErrorHandleProc) { return FALSE; }
BOOL __stdcall ExecutiveImpl::InitializeFileStorageWithoutRegistry(char*, DWORD, DWORD, DWORD, FILE_ACCESS_METHOD, PACKFILE_NAME_TABLE*, DWORD, BOOL) { return TRUE; }
BOOL __stdcall ExecutiveImpl::InitializeFileStorage(DWORD, DWORD, DWORD, FILE_ACCESS_METHOD, PACKFILE_NAME_TABLE*, DWORD, BOOL) { return TRUE; }
void __stdcall ExecutiveImpl::SetViewport(DWORD) {}
DWORD __stdcall ExecutiveImpl::IsValidHandle(GXMAP_OBJECT_HANDLE) { return 0; }
DWORD __stdcall ExecutiveImpl::GXOProcess(GXOBJECT_HANDLE, DWORD) { return 0; }
BOOL __stdcall ExecutiveImpl::GXORender(GXOBJECT_HANDLE) { return TRUE; }

// --- GXObject state methods ---
void __stdcall ExecutiveImpl::GXOSetCurrentFrame(GXOBJECT_HANDLE, DWORD) {}
void __stdcall ExecutiveImpl::GXOSetCurrentMotionIndex(GXOBJECT_HANDLE, DWORD) {}
BOOL __stdcall ExecutiveImpl::GXOGetMotionDesc(GXOBJECT_HANDLE, MOTION_DESC*, DWORD, DWORD) { return FALSE; }
DWORD __stdcall ExecutiveImpl::GXOGetMotionNum(GXOBJECT_HANDLE, DWORD) { return 0; }
DWORD __stdcall ExecutiveImpl::GXOGetCurrentFrame(GXOBJECT_HANDLE) { return 0; }
DWORD __stdcall ExecutiveImpl::GXOGetCurrentMotionIndex(GXOBJECT_HANDLE) { return 0; }
void __stdcall ExecutiveImpl::GXOSetCurrentMaterialIndex(GXOBJECT_HANDLE, DWORD) {}
DWORD __stdcall ExecutiveImpl::GXOGetMaterialNum(GXOBJECT_HANDLE, DWORD) { return 0; }
DWORD __stdcall ExecutiveImpl::GXOGetCurrentMaterialIndex(GXOBJECT_HANDLE) { return 0; }
void __stdcall ExecutiveImpl::GXOSetPosition(GXOBJECT_HANDLE, VECTOR3*, BOOL) {}
void __stdcall ExecutiveImpl::GXOMovePosition(GXOBJECT_HANDLE, VECTOR3*) {}
void __stdcall ExecutiveImpl::GXOGetPosition(GXOBJECT_HANDLE, VECTOR3*) {}
void __stdcall ExecutiveImpl::GXOMovePositionWithCollide(GXOBJECT_HANDLE, VECTOR3*) {}
void __stdcall ExecutiveImpl::GXOGetInterpolatedPosition(GXOBJECT_HANDLE, VECTOR3*) {}
void __stdcall ExecutiveImpl::GXOSetDirection(GXOBJECT_HANDLE, VECTOR3*, float) {}
void __stdcall ExecutiveImpl::GXOSetDirectionFPSStyle(GXOBJECT_HANDLE, VECTOR3*) {}
void __stdcall ExecutiveImpl::GXOChangeDirection(GXOBJECT_HANDLE, float) {}
void __stdcall ExecutiveImpl::GXOGetDirection(GXOBJECT_HANDLE, VECTOR3*, float*) {}
void __stdcall ExecutiveImpl::GXOGetDirectionFPSStyle(GXOBJECT_HANDLE, VECTOR3*) {}
void __stdcall ExecutiveImpl::GXOSetScale(GXOBJECT_HANDLE, VECTOR3*) {}
void __stdcall ExecutiveImpl::GXOGetScale(GXOBJECT_HANDLE, VECTOR3*) {}
void __stdcall ExecutiveImpl::GXOMoveForward(GXOBJECT_HANDLE, float) {}
void __stdcall ExecutiveImpl::GXOEnableHFieldApply(GXOBJECT_HANDLE) {}
void __stdcall ExecutiveImpl::GXODisableHFieldApply(GXOBJECT_HANDLE) {}
BOOL __stdcall ExecutiveImpl::GXOIsHFieldApply(GXOBJECT_HANDLE) { return FALSE; }
void __stdcall ExecutiveImpl::GXOEnableAsEffect(GXOBJECT_HANDLE) {}
void __stdcall ExecutiveImpl::GXODisableAsEffect(GXOBJECT_HANDLE) {}
BOOL __stdcall ExecutiveImpl::GXOIsAsEffect(GXOBJECT_HANDLE) { return FALSE; }
BOOL __stdcall ExecutiveImpl::GXOIsLanding(GXOBJECT_HANDLE) { return FALSE; }
void __stdcall ExecutiveImpl::GXOSetZOrder(GXOBJECT_HANDLE, int) {}
void __stdcall ExecutiveImpl::GXOEnableSelfIllumin(GXOBJECT_HANDLE) {}
void __stdcall ExecutiveImpl::GXODisableSelfIllumin(GXOBJECT_HANDLE) {}
BOOL __stdcall ExecutiveImpl::GXOIsEnableSelfIllumin(GXOBJECT_HANDLE) { return FALSE; }

// --- GXObject management ---
void __stdcall ExecutiveImpl::EstimatedMoveForward(GXOBJECT_HANDLE, VECTOR3*, float) {}
BOOL __stdcall ExecutiveImpl::GXOAttach(GXOBJECT_HANDLE, GXOBJECT_HANDLE, char*) { return FALSE; }
BOOL __stdcall ExecutiveImpl::GXOAttachWithModelIndex(GXOBJECT_HANDLE, GXOBJECT_HANDLE, DWORD, char*) { return FALSE; }
BOOL __stdcall ExecutiveImpl::GXODetach(GXOBJECT_HANDLE, GXOBJECT_HANDLE) { return FALSE; }
BOOL __stdcall ExecutiveImpl::GXOAttachLight(GXOBJECT_HANDLE, char*, GXLIGHT_HANDLE, VECTOR3*, DWORD) { return FALSE; }
BOOL __stdcall ExecutiveImpl::GXODetachLight(GXOBJECT_HANDLE, GXLIGHT_HANDLE) { return FALSE; }
BOOL __stdcall ExecutiveImpl::GXOAttachCameraFront(GXOBJECT_HANDLE, float) { return FALSE; }
BOOL __stdcall ExecutiveImpl::GXODetachCameraFront(GXOBJECT_HANDLE) { return FALSE; }
ULONG __stdcall ExecutiveImpl::GXOGetModel(GXOBJECT_HANDLE, I3DModel**, DWORD*, DWORD) { return 0; }
GXOBJECT_HANDLE __stdcall ExecutiveImpl::GXOGetObjectWithScreenCoord(VECTOR3*, DWORD*, DWORD*, float*, POINT*, DWORD, DWORD) { return 0; }
DWORD __stdcall ExecutiveImpl::GXOGetMultipleObjectWithScreenCoordRect(PICK_GXOBJECT_DESC*, DWORD, RECT*, DWORD, DWORD) { return 0; }
DWORD __stdcall ExecutiveImpl::GXOGetMultipleObjectWithScreenCoord(PICK_GXOBJECT_DESC*, DWORD, POINT*, DWORD, DWORD) { return 0; }
BOOL __stdcall ExecutiveImpl::GXOReplaceModel(GXOBJECT_HANDLE, DWORD, char*) { return FALSE; }
void __stdcall ExecutiveImpl::GXOGetCollisionMesh(GXOBJECT_HANDLE, COLLISION_MESH_OBJECT_DESC*) {}
DWORD __stdcall ExecutiveImpl::GXOGetObjectIndex(GXOBJECT_HANDLE, char*, DWORD) { return 0; }
BOOL __stdcall ExecutiveImpl::GXOGetWorldMatrixPerObject(GXOBJECT_HANDLE, MATRIX4*, DWORD, DWORD) { return FALSE; }
DWORD __stdcall ExecutiveImpl::GXOGetAllObjectsWitLoadMapScript(GXOBJECT_HANDLE*, DWORD) { return 0; }

// --- Render control ---
void __stdcall ExecutiveImpl::DisableRender(GXMAP_OBJECT_HANDLE) {}
void __stdcall ExecutiveImpl::EnableRender(GXMAP_OBJECT_HANDLE) {}
void __stdcall ExecutiveImpl::DisableSchedule(GXMAP_OBJECT_HANDLE) {}
void __stdcall ExecutiveImpl::EnableSchedule(GXMAP_OBJECT_HANDLE) {}
BOOL __stdcall ExecutiveImpl::IsRenderable(GXMAP_OBJECT_HANDLE) { return TRUE; }
void __stdcall ExecutiveImpl::SetPickType(GXMAP_OBJECT_HANDLE, PICK_TYPE) {}
PICK_TYPE __stdcall ExecutiveImpl::GetPickType(GXMAP_OBJECT_HANDLE) { return PICK_TYPE(0); }
void __stdcall ExecutiveImpl::SetRenderFlag(GXMAP_OBJECT_HANDLE, DWORD) {}
DWORD __stdcall ExecutiveImpl::GetRenderFlag(GXMAP_OBJECT_HANDLE) { return 0; }
void __stdcall ExecutiveImpl::SetAlphaFlag(GXMAP_OBJECT_HANDLE, DWORD) {}
DWORD __stdcall ExecutiveImpl::GetAlphaFlag(GXMAP_OBJECT_HANDLE) { return 0; }
void* __stdcall ExecutiveImpl::GetData(GXMAP_OBJECT_HANDLE) { return nullptr; }
void __stdcall ExecutiveImpl::SetData(GXMAP_OBJECT_HANDLE, void*) {}
DWORD __stdcall ExecutiveImpl::GetID(GXMAP_OBJECT_HANDLE) { return 0; }
BOOL __stdcall ExecutiveImpl::SetID(GXMAP_OBJECT_HANDLE, DWORD) { return FALSE; }
void __stdcall ExecutiveImpl::ReleaseID(GXMAP_OBJECT_HANDLE) {}
GXMAP_OBJECT_HANDLE __stdcall ExecutiveImpl::GetGXMapObjectWithID(DWORD) { return 0; }
void __stdcall ExecutiveImpl::SetPropertyFlag(GXMAP_OBJECT_HANDLE, DWORD) {}
DWORD __stdcall ExecutiveImpl::GetPropertyFlag(GXMAP_OBJECT_HANDLE) { return 0; }
void __stdcall ExecutiveImpl::EnablePick(GXMAP_OBJECT_HANDLE) {}
void __stdcall ExecutiveImpl::DisablePick(GXMAP_OBJECT_HANDLE) {}
void __stdcall ExecutiveImpl::EnableUpdateShading(GXMAP_OBJECT_HANDLE) {}
void __stdcall ExecutiveImpl::DisableUpdateShading(GXMAP_OBJECT_HANDLE) {}
void __stdcall ExecutiveImpl::LockTransform(GXMAP_OBJECT_HANDLE) {}
void __stdcall ExecutiveImpl::UnLockTransform(GXMAP_OBJECT_HANDLE) {}
BOOL __stdcall ExecutiveImpl::IsLockTransform(GXMAP_OBJECT_HANDLE) { return FALSE; }
void __stdcall ExecutiveImpl::EnableSendShadow(GXMAP_OBJECT_HANDLE) {}
void __stdcall ExecutiveImpl::DisableSendShadow(GXMAP_OBJECT_HANDLE) {}
BOOL __stdcall ExecutiveImpl::IsInViewVolume(GXMAP_OBJECT_HANDLE) { return TRUE; }
BOOL __stdcall ExecutiveImpl::IsCollisionWithScreenCoord(GXOBJECT_HANDLE, VECTOR3*, DWORD*, DWORD*, float*, POINT*, DWORD, DWORD) { return FALSE; }

// --- Frame ---
DWORD __stdcall ExecutiveImpl::GetFrameCount() { return m_dwFrameCount; }
BOOL __stdcall ExecutiveImpl::PreCreateLight(char*, DWORD) { return FALSE; }
DWORD __stdcall ExecutiveImpl::PreLoadGXObject(char*) { return 0; }
GXOBJECT_HANDLE __stdcall ExecutiveImpl::CreateGXObject(char*, GXSchedulePROC, void*, DWORD) { return 0; }
GXLIGHT_HANDLE __stdcall ExecutiveImpl::CreateGXLight(LIGHT_DESC*, GXLightSchedulePROC, void*, DWORD, MATERIAL*, DWORD) { return 0; }
GXTRIGGER_HANDLE __stdcall ExecutiveImpl::CreateGXEventTrigger(GXSchedulePROC, void*, DWORD) { return 0; }
GXMAP_HANDLE __stdcall ExecutiveImpl::CreateGXMap(GXMapSchedulePROC, void*, DWORD) { return 0; }
void __stdcall ExecutiveImpl::UnloadPreLoadedGXObject(GXOBJECT_HANDLE) {}
void __stdcall ExecutiveImpl::UnloadAllPreLoadedGXObject(DWORD) {}
BOOL __stdcall ExecutiveImpl::BeginBuildMap(DWORD) { return TRUE; }
BOOL __stdcall ExecutiveImpl::InsertHFieldToGXMap(IHeightField*) { return TRUE; }
BOOL __stdcall ExecutiveImpl::InsertStaticModelTOGXMap(I3DStaticModel*) { return TRUE; }
BOOL __stdcall ExecutiveImpl::EndBuildMap(float, float) { return TRUE; }
BOOL __stdcall ExecutiveImpl::RebuildMap(float, float) { return TRUE; }
BOOL __stdcall ExecutiveImpl::GetWorldBoundingBox(MAABB*) { return TRUE; }
DWORD __stdcall ExecutiveImpl::GetFramePerSec() { return m_dwGameFPS; }
void __stdcall ExecutiveImpl::SetFramePerSec(DWORD fps) { m_dwGameFPS = fps; }

// --- Light ---
void __stdcall ExecutiveImpl::GXLSetLightDesc(GXLIGHT_HANDLE, LIGHT_DESC*) {}
void __stdcall ExecutiveImpl::GXLGetLightDesc(GXLIGHT_HANDLE, LIGHT_DESC*) {}
void __stdcall ExecutiveImpl::GXLSetPosition(GXLIGHT_HANDLE, VECTOR3*) {}
void __stdcall ExecutiveImpl::GXLGetPosition(GXLIGHT_HANDLE, VECTOR3*) {}
void __stdcall ExecutiveImpl::GXLMovePosition(GXLIGHT_HANDLE, VECTOR3*) {}
GXLIGHT_HANDLE __stdcall ExecutiveImpl::GXLGetLightWithScreenCoord(VECTOR3*, float*, POINT*, DWORD, DWORD) { return 0; }
void __stdcall ExecutiveImpl::GXLDisableStaticShadow(GXLIGHT_HANDLE) {}
void __stdcall ExecutiveImpl::GXLEnableStaticShadow(GXLIGHT_HANDLE) {}
BOOL __stdcall ExecutiveImpl::GXLIsDisableStaticShadow(GXLIGHT_HANDLE) { return FALSE; }
void __stdcall ExecutiveImpl::GXLEnableDynamicLight(GXLIGHT_HANDLE) {}
void __stdcall ExecutiveImpl::GXLDisableDynamicLight(GXLIGHT_HANDLE) {}
BOOL __stdcall ExecutiveImpl::GXLIsEnableDynamicLight(GXLIGHT_HANDLE) { return FALSE; }

// --- Trigger ---
void __stdcall ExecutiveImpl::GXTSetPosition(GXTRIGGER_HANDLE, VECTOR3*) {}
void __stdcall ExecutiveImpl::GXTSetScale(GXTRIGGER_HANDLE, VECTOR3*) {}
void __stdcall ExecutiveImpl::GXTSetRotation(GXTRIGGER_HANDLE, VECTOR3*) {}
void __stdcall ExecutiveImpl::GXTGetEventTriggerDesc(GXTRIGGER_HANDLE, EVENT_TRIGGER_DESC*) {}
GXTRIGGER_HANDLE __stdcall ExecutiveImpl::GXTGetObjectWithScreenCoord(VECTOR3*, float*, POINT*, DWORD, DWORD) { return 0; }

// --- Map ---
BOOL __stdcall ExecutiveImpl::GXMGetCollisionPointWithScreenCoord(DWORD*, VECTOR3*, float*, POINT*, DWORD) { return FALSE; }
BOOL __stdcall ExecutiveImpl::GXMGetHFieldCollisionPointWithRay(VECTOR3*, float*, VECTOR3*, VECTOR3*) { return FALSE; }
BOOL __stdcall ExecutiveImpl::GXMGetHFieldCollisionPointWithScreenCoord(VECTOR3*, float*, POINT*) { return FALSE; }
BOOL __stdcall ExecutiveImpl::GXMGetHFieldHeight(float*, float, float) { return FALSE; }
BOOL __stdcall ExecutiveImpl::GXMStaticShadeMap(DWORD, LIGHT_DESC*, DWORD, DWORD, SHADE_FUNC) { return FALSE; }
void __stdcall ExecutiveImpl::GXMSetHFieldAlphaFlag(DWORD) {}
void __stdcall ExecutiveImpl::GXMSetSTMAlphaFlag(DWORD) {}

// --- Mode ---
BOOL __stdcall ExecutiveImpl::SetSymbol(SYMBOL_TYPE, char*) { return FALSE; }
void __stdcall ExecutiveImpl::SetRenderMode(DWORD mode) { m_dwRenderMode = mode; }
DWORD __stdcall ExecutiveImpl::GetRenderMode() { return m_dwRenderMode; }
VOID __stdcall ExecutiveImpl::SetPickingMode(DWORD mode) { m_dwPickingMode = mode; }
BOOL __stdcall ExecutiveImpl::DisableRenderAllStaticModels() { return TRUE; }
BOOL __stdcall ExecutiveImpl::EnableRenderAllStaticModels() { return TRUE; }

// --- Misc ---
void __stdcall ExecutiveImpl::GXOSetBoundingVolume(GXOBJECT_HANDLE, BOUNDING_VOLUME*) {}
DWORD __stdcall ExecutiveImpl::GetGXObjectsNum() { return 0; }
GXOBJECT_HANDLE __stdcall ExecutiveImpl::GetGXObjectWithSeqIndex(DWORD) { return 0; }
DWORD __stdcall ExecutiveImpl::GetGXLightsNum() { return 0; }
GXLIGHT_HANDLE __stdcall ExecutiveImpl::GetGXLightWithSeqIndex(DWORD) { return 0; }
DWORD __stdcall ExecutiveImpl::GetGXEventTriggersNum() { return 0; }
GXTRIGGER_HANDLE __stdcall ExecutiveImpl::GetGXEventTriggerWithSeqIndex(DWORD) { return 0; }
BOOL __stdcall ExecutiveImpl::LoadMapScript(char*, LOAD_CALLBACK_FUNC, DWORD) { return FALSE; }
void __stdcall ExecutiveImpl::DeleteAllGXMapObjectsWitLoadMapScript() {}

void __stdcall ExecutiveImpl::Render() { if (m_renderDevice) m_renderDevice->Clear(); }
BOOL __stdcall ExecutiveImpl::RenderCameraFrontObject() { return TRUE; }
DWORD __stdcall ExecutiveImpl::Process() { if (m_renderDevice) m_renderDevice->Clear(); ++m_dwFrameCount; return 0; }
DWORD __stdcall ExecutiveImpl::Run(DWORD, GX_FUNC, GX_FUNC, DWORD) { return 0; }
BOOL __stdcall ExecutiveImpl::SetCameraFitGXObject(GXOBJECT_HANDLE, float, float, float, DWORD) { return TRUE; }
void __stdcall ExecutiveImpl::SetHFieldDetail(DWORD) {}

// --- Delete ---
BOOL __stdcall ExecutiveImpl::DeleteGXObject(GXOBJECT_HANDLE) { return TRUE; }
BOOL __stdcall ExecutiveImpl::DeleteGXLight(GXLIGHT_HANDLE) { return TRUE; }
BOOL __stdcall ExecutiveImpl::DeleteGXEventTrigger(GXTRIGGER_HANDLE) { return TRUE; }
BOOL __stdcall ExecutiveImpl::DeleteGXMap(GXMAP_HANDLE) { return TRUE; }
BOOL __stdcall ExecutiveImpl::DeleteAllGXObjects() { return TRUE; }
BOOL __stdcall ExecutiveImpl::DeleteAllGXLights() { return TRUE; }
BOOL __stdcall ExecutiveImpl::DeleteAllGXEventTriggers() { return TRUE; }
BOOL __stdcall ExecutiveImpl::ImmDeleteGXObject(GXOBJECT_HANDLE) { return TRUE; }
BOOL __stdcall ExecutiveImpl::ImmDeleteGXLight(GXLIGHT_HANDLE) { return TRUE; }
BOOL __stdcall ExecutiveImpl::ImmDeleteGXEventTrigger(GXTRIGGER_HANDLE) { return TRUE; }
BOOL __stdcall ExecutiveImpl::ImmDeleteGXDecal(GXDECAL_HANDLE) { return TRUE; }
BOOL __stdcall ExecutiveImpl::DefDeleteGXObject(GXOBJECT_HANDLE) { return TRUE; }
BOOL __stdcall ExecutiveImpl::DefDeleteGXLight(GXLIGHT_HANDLE) { return TRUE; }
BOOL __stdcall ExecutiveImpl::DefDeleteGXEventTrigger(GXTRIGGER_HANDLE) { return TRUE; }
BOOL __stdcall ExecutiveImpl::DefDeleteGXDecal(GXTRIGGER_HANDLE) { return TRUE; }
GXDECAL_HANDLE __stdcall ExecutiveImpl::CreateGXDecal(DECAL_DESC*, GXDecalSchedulePROC, void*, DWORD) { return 0; }
BOOL __stdcall ExecutiveImpl::DeleteGXDecal(GXDECAL_HANDLE) { return TRUE; }
BOOL __stdcall ExecutiveImpl::GetMapReadCount(char*) { return FALSE; }
BOOL __stdcall ExecutiveImpl::GXOInitializeIllusionEffect(GXOBJECT_HANDLE, DWORD, char*, void*, DWORD) { return FALSE; }
void __stdcall ExecutiveImpl::GXOBeginIllusionEffect(GXOBJECT_HANDLE) {}
void __stdcall ExecutiveImpl::GXOEndIllusionEffect(GXOBJECT_HANDLE) {}
DWORD __stdcall ExecutiveImpl::GXOGetAttachedGXObjects(GXOBJECT_HANDLE, GXOBJECT_HANDLE*, DWORD) { return 0; }
void __stdcall ExecutiveImpl::GXOSetScheduleProc(GXOBJECT_HANDLE, GXSchedulePROC) {}
GXSchedulePROC __stdcall ExecutiveImpl::GXOGetScheduleProc(GXOBJECT_HANDLE) { return nullptr; }
void __stdcall ExecutiveImpl::GXOSetEffectIndex(GXOBJECT_HANDLE, DWORD) {}
DWORD __stdcall ExecutiveImpl::GXOGetEffectIndex(GXOBJECT_HANDLE) { return 0; }
void __stdcall ExecutiveImpl::SetAfterInterpolation(AfterInterpolationCallBack) {}
DWORD __stdcall ExecutiveImpl::GetCurrentGameFrame() { return m_dwFrameCount; }
ULONG __stdcall ExecutiveImpl::GXMGetStaticModel(I3DStaticModel**) { return 0; }
ULONG __stdcall ExecutiveImpl::GXMGetHField(IHeightField**) { return 0; }
void __stdcall ExecutiveImpl::GXODisableUnloadPreLoaded(GXOBJECT_HANDLE) {}
void __stdcall ExecutiveImpl::GXOEnableUnloadPreLoaded(GXOBJECT_HANDLE) {}
void __stdcall ExecutiveImpl::GXOGetLastVelocityAfterCollisionTest(GXOBJECT_HANDLE, VECTOR3*) {}
void __stdcall ExecutiveImpl::GXOEnableScaleOfAttachedModel(GXOBJECT_HANDLE) {}
void __stdcall ExecutiveImpl::GXODisableScaleOfAttachedModel(GXOBJECT_HANDLE) {}
void __stdcall ExecutiveImpl::SetModelLODUsingMode(DWORD, DWORD) {}
DWORD __stdcall ExecutiveImpl::GetModelLODUsingMode() { return 0; }
DWORD __stdcall ExecutiveImpl::GetModelLodUsingLevel() { return 0; }
MATRIX4 __stdcall ExecutiveImpl::GXOGetParentMatrix(GXOBJECT_HANDLE) { MATRIX4 m; memset(&m, 0, sizeof(m)); return m; }
BOOL __stdcall ExecutiveImpl::GXOIsCollisionMeshWithBoundingBox(GXOBJECT_HANDLE, VECTOR3*, DWORD*, DWORD*, float*, COLLISION_MESH_OBJECT_DESC*) { return FALSE; }
BOOL __stdcall ExecutiveImpl::GXOIsCollisionMeshWithObjectBoundingBox(GXOBJECT_HANDLE, VECTOR3*, DWORD*, DWORD*, float*, GXOBJECT_HANDLE) { return FALSE; }
BOOL __stdcall ExecutiveImpl::GXOIsCollisionBoungingBoxWithScreenCoord(GXOBJECT_HANDLE, VECTOR3*, VECTOR3*, POINT*, DWORD) { return FALSE; }
DWORD __stdcall ExecutiveImpl::GXOGetModelNum(GXOBJECT_HANDLE) { return 0; }
DWORD __stdcall ExecutiveImpl::GXOGetObjectNum(GXOBJECT_HANDLE, DWORD) { return 0; }
BOOL __stdcall ExecutiveImpl::GXOGetCollisononObjectDesc(GXOBJECT_HANDLE, COLLISION_MESH_OBJECT_DESC*, DWORD, DWORD) { return FALSE; }
BOOL __stdcall ExecutiveImpl::GXOGetCollisionModelDesc(GXOBJECT_HANDLE, COLLISION_MODEL_DESC*, DWORD) { return FALSE; }
BOOL __stdcall ExecutiveImpl::GXOGetWorldMatrixByName(GXOBJECT_HANDLE, MATRIX4*, char*) { return FALSE; }
