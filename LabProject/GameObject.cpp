#include "StdAfx.h"
#include "GameObject.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
inline float RandF(float fMin, float fMax)
{
	return(fMin + ((float)rand() / (float)RAND_MAX) * (fMax - fMin));
}

XMVECTOR RandomUnitVectorOnSphere()
{
	XMVECTOR xmvOne = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR xmvZero = XMVectorZero();

	while (true)
	{
		XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);
		if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne)) return(XMVector3Normalize(v));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGameObject::CGameObject() 
{ 
	m_pMesh = NULL; 
	m_xmf4x4World = Matrix4x4::Identity();

	m_dwColor = RGB(0, 0, 0);

	m_xmf3MovingDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_fMovingSpeed = 0.0f;
	m_fMovingRange = 0.0f;

	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 0.05f;
}

CGameObject::CGameObject(CMesh *pMesh) : CGameObject()
{
	m_pMesh = pMesh; 
}

CGameObject::~CGameObject(void)
{
	if (m_pMesh) m_pMesh->Release();
}

void CGameObject::SetPosition(float x, float y, float z) 
{
	m_xmf4x4World._41 = x; 
	m_xmf4x4World._42 = y; 
	m_xmf4x4World._43 = z; 
}

void CGameObject::SetPosition(XMFLOAT3& xmf3Position) 
{ 
	m_xmf4x4World._41 = xmf3Position.x; 
	m_xmf4x4World._42 = xmf3Position.y; 
	m_xmf4x4World._43 = xmf3Position.z; 
}

XMFLOAT3 CGameObject::GetPosition() 
{ 
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43)); 
}

XMFLOAT3 CGameObject::GetLook() 
{ 	
	XMFLOAT3 xmf3LookAt(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);
	xmf3LookAt = Vector3::Normalize(xmf3LookAt);
	return(xmf3LookAt);
}

XMFLOAT3 CGameObject::GetUp() 
{ 	
	XMFLOAT3 xmf3Up(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	xmf3Up = Vector3::Normalize(xmf3Up);
	return(xmf3Up);
}

XMFLOAT3 CGameObject::GetRight()
{ 	
	XMFLOAT3 xmf3Right(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	xmf3Right = Vector3::Normalize(xmf3Right);
	return(xmf3Right);
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Right, fDistance));
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Up, fDistance));
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3LookAt = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3LookAt, fDistance));
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(fPitch, fYaw, fRoll);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT3& xmf3RotationAxis, float fAngle)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationAxis(xmf3RotationAxis, fAngle);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Move(XMFLOAT3& vDirection, float fSpeed)
{
	SetPosition(m_xmf4x4World._41 + vDirection.x * fSpeed, m_xmf4x4World._42 + vDirection.y * fSpeed, m_xmf4x4World._43 + vDirection.z * fSpeed);
}

void CGameObject::LookTo(XMFLOAT3& xmf3LookTo, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(GetPosition(), xmf3LookTo, xmf3Up);
	m_xmf4x4World._11 = xmf4x4View._11; m_xmf4x4World._12 = xmf4x4View._21; m_xmf4x4World._13 = xmf4x4View._31;
	m_xmf4x4World._21 = xmf4x4View._12; m_xmf4x4World._22 = xmf4x4View._22; m_xmf4x4World._23 = xmf4x4View._32;
	m_xmf4x4World._31 = xmf4x4View._13; m_xmf4x4World._32 = xmf4x4View._23; m_xmf4x4World._33 = xmf4x4View._33;
}

void CGameObject::LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(GetPosition(), xmf3LookAt, xmf3Up);
	m_xmf4x4World._11 = xmf4x4View._11; m_xmf4x4World._12 = xmf4x4View._21; m_xmf4x4World._13 = xmf4x4View._31;
	m_xmf4x4World._21 = xmf4x4View._12; m_xmf4x4World._22 = xmf4x4View._22; m_xmf4x4World._23 = xmf4x4View._32;
	m_xmf4x4World._31 = xmf4x4View._13; m_xmf4x4World._32 = xmf4x4View._23; m_xmf4x4World._33 = xmf4x4View._33;
}

void CGameObject::UpdateBoundingBox()
{
	if (m_pMesh)
	{
		m_pMesh->m_xmOOBB.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
		XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
	}
}

void CGameObject::Animate(float fElapsedTime)
{
	if (m_fRotationSpeed != 0.0f) Rotate(m_xmf3RotationAxis, m_fRotationSpeed * fElapsedTime);
	if (m_fMovingSpeed != 0.0f) Move(m_xmf3MovingDirection, m_fMovingSpeed * fElapsedTime);	

	UpdateBoundingBox();
}

void CGameObject::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	if (m_pMesh)
	{
		if (pCamera->IsInFrustum(m_xmOOBB))
		{
			// 1. 펜(선 색상)과 브러시(면 색상)를 모두 생성
			HPEN hPen = ::CreatePen(PS_SOLID, 1, m_dwColor);
			HBRUSH hBrush = ::CreateSolidBrush(m_dwColor);

			// 2. hDC에 선택
			HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
			HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDCFrameBuffer, hBrush);

			// 3. 메시 렌더링 호출
			m_pMesh->Render(hDCFrameBuffer, m_xmf4x4World, pCamera);

			// 4. 이전 객체로 복구 및 생성한 객체 삭제
			::SelectObject(hDCFrameBuffer, hOldPen);
			::SelectObject(hDCFrameBuffer, hOldBrush);
			::DeleteObject(hPen);
			::DeleteObject(hBrush);
		}
	}
}

void CGameObject::GenerateRayForPicking(XMVECTOR& xmvPickPosition, XMMATRIX& xmmtxView, XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection)
{
	XMMATRIX xmmtxToModel = XMMatrixInverse(NULL, XMLoadFloat4x4(&m_xmf4x4World) * xmmtxView);

	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	xmvPickRayOrigin = XMVector3TransformCoord(XMLoadFloat3(&xmf3CameraOrigin), xmmtxToModel);
	xmvPickRayDirection = XMVector3TransformCoord(xmvPickPosition, xmmtxToModel);
	xmvPickRayDirection = XMVector3Normalize(xmvPickRayDirection - xmvPickRayOrigin);
}

int CGameObject::PickObjectByRayIntersection(XMVECTOR& xmvPickPosition, XMMATRIX& xmmtxView, float *pfHitDistance)
{
	int nIntersected = 0;
	if (m_pMesh)
	{
		XMVECTOR xmvPickRayOrigin, xmvPickRayDirection;
		GenerateRayForPicking(xmvPickPosition, xmmtxView, xmvPickRayOrigin, xmvPickRayDirection);
		nIntersected = m_pMesh->CheckRayIntersection(xmvPickRayOrigin, xmvPickRayDirection, pfHitDistance);
	}
	return(nIntersected);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CWallsObject::CWallsObject()
{
}

CWallsObject::~CWallsObject()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
XMFLOAT3 CExplosiveObject::m_pxmf3SphereVectors[EXPLOSION_DEBRISES];
CMesh *CExplosiveObject::m_pExplosionMesh = NULL;

CExplosiveObject::CExplosiveObject()
{
	CCubeMesh* pBulletMesh = new CCubeMesh(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < ENEMY_BULLETS; i++)
	{
		mm_ppBullets[i] = new CBulletObject(150.0f);


		mm_ppBullets[i]->SetMesh(pBulletMesh);

		mm_ppBullets[i]->SetActive(false);
	}
	m_fFireTimer = (float)(rand() % 5000) / 1000.0f;
}

CExplosiveObject::~CExplosiveObject()
{
	for (int i = 0; i < ENEMY_BULLETS; i++) delete mm_ppBullets[i];
}

void CExplosiveObject::PrepareExplosion()
{
	for (int i = 0; i < 40; i++) XMStoreFloat3(&m_pxmf3SphereVectors[i], ::RandomUnitVectorOnSphere());

	m_pExplosionMesh = new CCubeMesh(0.5f, 0.5f, 0.5f);
}
void CExplosiveObject::Animate(float fElapsedTime)
{

	if (m_bBlowingUp)
	{
		
		m_fElapsedTimes += fElapsedTime;
		if (m_fElapsedTimes <= m_fDuration)
		{
			XMFLOAT3 xmf3Position = GetPosition();
			for (int i = 0; i < 40; i++)
			{
				m_pxmf4x4Transforms[i] = Matrix4x4::Identity();
				m_pxmf4x4Transforms[i]._41 = xmf3Position.x + m_pxmf3SphereVectors[i].x * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._42 = xmf3Position.y + m_pxmf3SphereVectors[i].y * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._43 = xmf3Position.z + m_pxmf3SphereVectors[i].z * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i] = Matrix4x4::Multiply(Matrix4x4::RotationAxis(m_pxmf3SphereVectors[i], m_fExplosionRotation * m_fElapsedTimes), m_pxmf4x4Transforms[i]);
			}
		}
		else
		{
			m_bBlowingUp = false;
			m_fElapsedTimes = 0.0f;
		}
	}
	else
	{
		XMFLOAT3 xmf3MyPosition = GetPosition();
		XMFLOAT3 xmf3Direction = Vector3::Subtract(P_distance, xmf3MyPosition);
		float fDistance = Vector3::Length(xmf3Direction);

		if (fDistance > 20.0f)
		{
			// 플레이어에게 다가감
			xmf3Direction = Vector3::Normalize(xmf3Direction);
			SetMovingDirection(xmf3Direction);
			SetMovingSpeed(10.0f); 
		}
		else
		{

			
		}
		
		m_fFireTimer += fElapsedTime;
		if (m_fFireTimer >= 5.0f) // 1초마다 발사
		{
			FireBullett(P_distance); // 플레이어 위치로 발사
			m_fFireTimer = 0.0f;     // 타이머 초기화
		}
		
		CGameObject::Animate(fElapsedTime);
	}
	for (int i = 0; i < ENEMY_BULLETS; i++)
	{
		if (mm_ppBullets[i]->m_bActive)
		{
			mm_ppBullets[i]->enemyAnimate(fElapsedTime);
		}
	}
	UpdateBoundingBox();
}
void CExplosiveObject::FireBullett(XMFLOAT3 xmf3PlayerPosition)
{
	CBulletObject* pBulletObject = NULL;

	for (int i = 0; i < ENEMY_BULLETS; i++) {
		if (!mm_ppBullets[i]->m_bActive) {
			pBulletObject = mm_ppBullets[i];
			break;
		}
	}

	if (pBulletObject)
	{
		XMFLOAT3 xmf3MyPosition = GetPosition();

	
		XMFLOAT3 xmf3Direction = Vector3::Subtract(xmf3PlayerPosition, xmf3MyPosition);
		xmf3Direction = Vector3::Normalize(xmf3Direction);

		XMFLOAT3 xmf3FirePosition = Vector3::Add(xmf3MyPosition, Vector3::ScalarProduct(xmf3Direction, 2.0f, false));

		pBulletObject->m_xmf4x4World = Matrix4x4::Identity(); // 월드 행렬 초기화
		pBulletObject->m_xmf4x4World._41 = xmf3FirePosition.x;
		pBulletObject->m_xmf4x4World._42 = xmf3FirePosition.y;
		pBulletObject->m_xmf4x4World._43 = xmf3FirePosition.z;

		pBulletObject->SetFirePosition(xmf3FirePosition);
		pBulletObject->SetMovingDirection(xmf3Direction);
		pBulletObject->SetMovingSpeed(10.0f);
		pBulletObject->SetActive(true);
	
	}
}
void CExplosiveObject::Render(HDC hDCFrameBuffer, CCamera *pCamera)
{
	if (m_bBlowingUp)
	{
		for (int i = 0; i < EXPLOSION_DEBRISES; i++)
		{
			if (m_pExplosionMesh)
			{
				HPEN hPen = ::CreatePen(PS_SOLID, 0, m_dwColor);
				HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
				m_pExplosionMesh->Render(hDCFrameBuffer, m_pxmf4x4Transforms[i], pCamera);
				::SelectObject(hDCFrameBuffer, hOldPen);
				::DeleteObject(hPen);
			}
		}
	}
	else
	{
		CGameObject::Render(hDCFrameBuffer, pCamera);
	}
	
	for (int i = 0; i < ENEMY_BULLETS; i++)
	{
		if (mm_ppBullets[i]->m_bActive)
		{
			mm_ppBullets[i]->Render(hDCFrameBuffer, pCamera);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CBulletObject::CBulletObject(float fEffectiveRange)
{
	m_fBulletEffectiveRange = fEffectiveRange;
	this->SetColor(255);
}

CBulletObject::~CBulletObject()
{
}

void CBulletObject::SetFirePosition(XMFLOAT3 xmf3FirePosition)
{ 
	m_xmf3FirePosition = xmf3FirePosition; 
	SetPosition(xmf3FirePosition);
}

void CBulletObject::Animate(float fElapsedTime)
{
	float fDistance = m_fMovingSpeed * fElapsedTime;
#ifdef _WITH_VECTOR_OPERATION
	XMFLOAT3 xmf3Position = GetPosition();

	m_fRotationAngle += m_fRotationSpeed * fElapsedTime;
	if (m_fRotationAngle > 360.0f) m_fRotationAngle = m_fRotationAngle - 360.0f;

	XMFLOAT4X4 mtxRotate1 = Matrix4x4::RotationYawPitchRoll(0.0f, m_fRotationAngle, 0.0f);

	XMFLOAT3 xmf3RotationAxis = Vector3::CrossProduct(m_xmf3RotationAxis, m_xmf3MovingDirection, true);
	float fDotProduct = Vector3::DotProduct(m_xmf3RotationAxis, m_xmf3MovingDirection);
	float fRotationAngle = ::IsEqual(fDotProduct, 1.0f) ? 0.0f : (float)XMConvertToDegrees(acos(fDotProduct));
	XMFLOAT4X4 mtxRotate2 = Matrix4x4::RotationAxis(xmf3RotationAxis, fRotationAngle);

	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate1, mtxRotate2);

	XMFLOAT3 xmf3Movement = Vector3::ScalarProduct(m_xmf3MovingDirection, fDistance, false);
	xmf3Position = Vector3::Add(xmf3Position, xmf3Movement);
	SetPosition(xmf3Position);
#else
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(0.0f, m_fRotationSpeed * fElapsedTime, 0.0f);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
	XMFLOAT3 xmf3Movement = Vector3::ScalarProduct(m_xmf3MovingDirection, fDistance, false);
	XMFLOAT3 xmf3Position = GetPosition();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Movement);
	SetPosition(xmf3Position);
#endif

	UpdateBoundingBox();

	if (Vector3::Distance(m_xmf3FirePosition, GetPosition()) > m_fBulletEffectiveRange) SetActive(false);
}

void CBulletObject::enemyAnimate(float fElapsedTime)
{

	if (m_fMovingSpeed > 0.0f)
	{
		XMFLOAT3 xmf3Shift = Vector3::ScalarProduct(m_xmf3MovingDirection, m_fMovingSpeed * fElapsedTime);

		
		XMFLOAT3 xmf3Pos = GetPosition();
		xmf3Pos = Vector3::Add(xmf3Pos, xmf3Shift);
		SetPosition(xmf3Pos);
	}
	UpdateBoundingBox();
	XMFLOAT3 xmf3Dist = Vector3::Subtract(GetPosition(), m_xmf3FirePosition);
	if (Vector3::Length(xmf3Dist) > m_fBulletEffectiveRange)
	{
		SetActive(false);
		SetPosition(0.0f, -1000.0f, 0.0f); // 화면 밖으로 숨김
	}
}