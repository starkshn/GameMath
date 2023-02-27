
#include "Precompiled.h"
#include "SoftRenderer.h"
#include <random>
using namespace CK::DD;

// ���ڸ� �׸��� �Լ�
void SoftRenderer::DrawGizmo2D()
{
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// �׸��� ����
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// ���� ���� ���
	Vector2 viewPos = g.GetMainCamera().GetTransform().GetPosition();
	Vector2 extent = Vector2(_ScreenSize.X * 0.5f, _ScreenSize.Y * 0.5f);

	// ���� �ϴܿ������� ���� �׸���
	int xGridCount = _ScreenSize.X / _Grid2DUnit;
	int yGridCount = _ScreenSize.Y / _Grid2DUnit;

	// �׸��尡 ���۵Ǵ� ���ϴ� ��ǥ �� ���
	Vector2 minPos = viewPos - extent;
	Vector2 minGridPos = Vector2(ceilf(minPos.X / (float)_Grid2DUnit), ceilf(minPos.Y / (float)_Grid2DUnit)) * (float)_Grid2DUnit;
	ScreenPoint gridBottomLeft = ScreenPoint::ToScreenCoordinate(_ScreenSize, minGridPos - viewPos);

	for (int ix = 0; ix < xGridCount; ++ix)
	{
		r.DrawFullVerticalLine(gridBottomLeft.X + ix * _Grid2DUnit, gridColor);
	}

	for (int iy = 0; iy < yGridCount; ++iy)
	{
		r.DrawFullHorizontalLine(gridBottomLeft.Y - iy * _Grid2DUnit, gridColor);
	}

	ScreenPoint worldOrigin = ScreenPoint::ToScreenCoordinate(_ScreenSize, -viewPos);
	r.DrawFullHorizontalLine(worldOrigin.Y, LinearColor::Red);
	r.DrawFullVerticalLine(worldOrigin.X, LinearColor::Green);
}

// ���� ������Ʈ ���


// ���� �� �ε��� ����ϴ� �Լ�
void SoftRenderer::LoadScene2D()
{
	// ���� �� �ε����� ����ϴ� ��� �� �ֿ� ���۷���
	auto& g = Get2DGameEngine();

}

// ���� ������ ������ ������ �����ϴ� ����
float fovAngle = 60.f;
Vector2 PlayerPosition(0.f, 0.f);
LinearColor PlayerColor = LinearColor::Gray;
Vector2 TargetPosition(0.f, 100.f);
LinearColor TargetColor = LinearColor::Blue;

// ���� ������ ����ϴ� �Լ�
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	// ���� �������� ����ϴ� ��� �� �ֿ� ���۷���
	auto& g = Get2DGameEngine();
	const InputManager& input = g.GetInputManager();

	// ���� ������ ���� ����
	static float moveSpeed = 100.f;
	static std::random_device rd;
	static std::mt19937 mt(rd());
	static std::uniform_real_distribution<float> randomPosX(-300.f, 300.f);
	static std::uniform_real_distribution<float> randomPosY(-200.f, 200.f);
	static float duration = 3.f;
	static float elapsedTime = 0.f;
	static Vector2 TargetStart = TargetPosition;
	static Vector2 TargetDestination = Vector2(randomPosX(mt), randomPosY(mt));

	// �þ߰��� cos ���� ���� �� ȸ�� ����� �����Ѵ�.
	static float halfFovCos = cosf(Math::Deg2Rad(fovAngle * 0.5f));

	elapsedTime = Math::Clamp(elapsedTime + InDeltaSeconds, 0.f, duration);

	// ������ �ð��� ����ϸ� ���ο� �̵� ������ �����ϰ� ����
	if (elapsedTime == duration)
	{
		TargetStart = TargetDestination;
		TargetPosition = TargetDestination;
		TargetDestination = Vector2(randomPosX(mt), randomPosY(mt));

		elapsedTime = 0.f;
	}
	else // ������ ���� ��ǥ�������� ���� �����ϸ鼭 �̵�
	{
		float ratio = elapsedTime / duration;
		TargetPosition = Vector2
		(
			Math::Lerp(TargetStart.X, TargetDestination.X, ratio),
			Math::Lerp(TargetStart.Y, TargetDestination.Y, ratio)
		);
	}

	Vector2 inputVector = Vector2(input.GetAxis(InputAxis::XAxis), input.GetAxis(InputAxis::YAxis)).GetNormalize();
	Vector2 deltaPosition = inputVector * moveSpeed * InDeltaSeconds;

	Vector2 f = Vector2::UnitY;
	Vector2 v = (TargetPosition - PlayerPosition).GetNormalize();

	float L = (TargetPosition - PlayerPosition).Size();

	// ��ü�� �������� ����
	if (v.Dot(f) >= halfFovCos && L < 300.f)
	{
		PlayerColor = LinearColor::Red;
		TargetColor = LinearColor::Red;
	}
	else
	{
		PlayerColor = LinearColor::Gray;
		TargetColor = LinearColor::Blue;
	}

	PlayerPosition += deltaPosition;
}

// ������ ������ ����ϴ� �Լ�
void SoftRenderer::Render2D()
{
	// ������ �������� ����ϴ� ��� �� �ֿ� ���۷���
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// ������ ������ ���� ����
	static float radius = 5.f;
	static std::vector<Vector2> sphere;
	static float sightLength = 300.f;

	if (sphere.empty())
	{
		for (float x = -radius; x <= radius; ++x)
		{
			for (float y = -radius; y <= radius; ++y)
			{
				Vector2 target(x, y);
				float sizeSquared = target.SizeSquared();
				float rr = radius * radius;
				if (sizeSquared < rr)
				{
					sphere.push_back(target);
				}
			}
		}
	}

	// �÷��̾� ������.  
	float halfFovSin = 0.f, halfFovCos = 0.f;
	Math::GetSinCos(halfFovSin, halfFovCos, fovAngle * 0.5f);

	r.DrawLine(PlayerPosition, PlayerPosition + Vector2(sightLength * halfFovSin, sightLength * halfFovCos), PlayerColor);
	r.DrawLine(PlayerPosition, PlayerPosition + Vector2(-sightLength * halfFovSin, sightLength * halfFovCos), PlayerColor);
	r.DrawLine(PlayerPosition, PlayerPosition + Vector2::UnitY * sightLength * 0.2f, PlayerColor);
	for (auto const& v : sphere)
	{
		r.DrawPoint(v + PlayerPosition, PlayerColor);
	}

	// Ÿ�� ������
	for (auto const& v : sphere)
	{
		r.DrawPoint(v + TargetPosition, TargetColor);
	}

	// �ֿ� ���� ���
	r.PushStatisticText(std::string("Player Position : ") + PlayerPosition.ToString());
	r.PushStatisticText(std::string("Target Position : ") + TargetPosition.ToString());
}

// �޽ø� �׸��� �Լ�
void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
}

// �ﰢ���� �׸��� �Լ�
void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
}
