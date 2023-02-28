
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
Vector2 point(0.f, 250.f);
Vector2 lineStart(-400.f, 0.f);
Vector2 lineEnd(400.f, 0.f);

// ���� ������ ����ϴ� �Լ�
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	// ���� �������� ����ϴ� ��� �� �ֿ� ���۷���
	auto& g = Get2DGameEngine();
	const InputManager& input = g.GetInputManager();

	// ���� ������ ���� ����
	static float duration = 6.f;
	static float elapsedTime = 0.f;
	static float currentDegree = 0.f;
	static float rotateSpeed = 180.f;
	static float distance = 250.f;
	
	static std::random_device rd;
	static std::mt19937 mt(rd());
	static std::uniform_real_distribution<float> randomY(-200.f, 200.f);

	elapsedTime = Math::Clamp(elapsedTime + InDeltaSeconds, 0.f, duration);
	if (elapsedTime == duration)
	{
		lineStart = Vector2(-400.f, randomY(mt));
		lineEnd = Vector2(400.f, randomY(mt));
		elapsedTime = 0.f;
	}

	// ���� ��ġ����
	currentDegree = Math::FMod(currentDegree + rotateSpeed * InDeltaSeconds, 360.f);
	float sin = 0.f, cos = 0.f;
	Math::GetSinCos(sin, cos, currentDegree);
	point = Vector2(cos, sin) * distance;
}

// ������ ������ ����ϴ� �Լ�
void SoftRenderer::Render2D()
{
	// ������ �������� ����ϴ� ��� �� �ֿ� ���۷���
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// ������ ������ ���� ����
	static float radius = 5.f;
	static std::vector<Vector2> circle;

	if (circle.empty())
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
					circle.push_back(target);
				}
			}
		}
	}

	// ���� ������ �� �׸���
	for (auto const& v : circle)
	{
		r.DrawPoint(v + point, LinearColor::Red);
	}

	// ������ ���� �׸���
	r.DrawLine(lineStart, lineEnd, LinearColor::Black);
	r.DrawLine(lineStart, point, LinearColor::Red);

	// ������ ��ġ�� �Ÿ� ���
	Vector2 unitV = (lineEnd - lineStart).GetNormalize();	// ������ ������ ���� ����
	Vector2 u = point - lineStart;							// �������� ����
	Vector2 projV = unitV * (u.Dot(unitV));					// u Dot unitV => ������ ũ�� ���� -> �̰��� �������Ϳ��ٰ� ����. �� ũ�⸸ŭ Ŀ��.
	Vector2 projectedPoint = lineStart + projV;				// �� ��ġ�� lineStart���ٰ� ���ؼ� ��Ȯ�ϰ� ��ġ��Ŵ
	float distance = (projectedPoint - point).Size();

	// ������ �� �׸���
	for (auto const& v : circle)
	{
		r.DrawPoint(v + projectedPoint, LinearColor::Magenta);
	}

	// ���� ���� �׸���
	r.DrawLine(projectedPoint, point, LinearColor::Gray);

	// ���� ������ ȭ�� ���
	r.PushStatisticText(std::string("Point : ") + point.ToString());
	r.PushStatisticText(std::string("Projected Point : ") + projectedPoint.ToString());
	r.PushStatisticText(std::string("Distance : ") + std::to_string(distance));
}

// �޽ø� �׸��� �Լ�
void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
}

// �ﰢ���� �׸��� �Լ�
void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
}
