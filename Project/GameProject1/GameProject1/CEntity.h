#pragma once

class CEntity
{
public:
	CEntity();
	CEntity(D3DXVECTOR3 pos, D3DXVECTOR3 cen, D3DXVECTOR3 dir, float speed) {
		Set(pos, cen, dir, speed);
	}
	virtual ~CEntity();

	void Set(D3DXVECTOR3 pos, D3DXVECTOR3 cen, D3DXVECTOR3 dir, float speed) {
		m_pos = pos;
		m_cen = cen;
		m_dir = dir;
		m_speed = speed;
	}

	const D3DXVECTOR3& getPos() { return m_pos; }
	const D3DXVECTOR3& getDir() { return m_dir; }
	const D3DXVECTOR3& getCen() { return m_cen; }
	float getSpeed() const { return m_speed; }

	void SetDir(const D3DXVECTOR3& dir)
	{
		m_dir = dir;
	}

	void SetSpeed(float speed) 
	{
		m_speed = speed;
	}

	void Update(float deltaTime)
	{
		m_pos = m_pos + m_dir * m_speed * deltaTime;
	}

	void SetPos(D3DXVECTOR3 pos)
	{
		m_pos = pos;
	}

protected:
	D3DXVECTOR3 m_pos;
	D3DXVECTOR3 m_dir;
	D3DXVECTOR3 m_cen;
	float m_speed;
};