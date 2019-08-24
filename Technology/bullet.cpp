// ----------------------------------------
//
// �e�����̐���[bullet.cpp]
// Author : Koki Nishiyama
//
// ----------------------------------------

// ----------------------------------------
//
// �C���N���[�h�t�@�C��
//
// ----------------------------------------
#include "bullet.h"
#include "effect.h"
#include "explosion.h"
#include "score.h"
#include "player.h"
#include "frame.h"
#include "array.h"
#include "number.h"
#include "box.h"

// ----------------------------------------
//
// �}�N����`
//
// ----------------------------------------
#define BULLETSIZE (50.0f)
#define BULLET_COLLISIONSIZE (40.0f)
#define BULLETSIX (D3DX_PI * 2 / CBullet::DIRECTION_MAX)
#define BULLETSIX_ONE (BULLETSIX / 2)
#define BULLET_CONNECT (3)
#define BULLET_MOVE (10.0f)
#define BULLET_CNTMOVE (50)

// ----------------------------------------
//
// �O���[�o���ϐ�
//
// ----------------------------------------

// ----------------------------------------
//
// �ÓI�ϐ��錾
//
// ----------------------------------------
LPDIRECT3DTEXTURE9 CBullet::m_pTex[CBullet::TYPE_MAX] = {};
int CBullet::m_nCntJudg = 0;

// ----------------------------------------
// �R���X�g���N�^����
// ----------------------------------------
CBullet::CBullet() : CScene_TWO::CScene_TWO(ACTOR_BULLET,LAYER_BULLET)
{
	/* �ϐ��̏����� */
	// �ʒu
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	// �ړI�ʒu
	m_posgoal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	// �ړ���
	m_move = D3DXVECTOR3(BULLET_MOVE, BULLET_MOVE, 0.0f);
	// �^�C�v
	m_type = TYPE_BEE;
	// �ł����ォ�O��
	m_state = STATE_BOX;
	// ��]��
	m_fRot = D3DX_PI;
	// �̗�
	m_nLife = 50;
	// �s�ԍ�
	m_nLine = 7;
	// ��ԍ�
	m_nColumn = 9;
	// �ړ��J�E���g
	m_nCntJudg = 0;
	// �Ȃ����Ă��锻��
	m_bJudg = false;
	// �����Ă锻��
	m_bDown = false;
	// �ړ����Ă��邩�ǂ���
	m_bMove = false;
	// �v���C���[�ԍ�
	m_PlayerId = CManager::PLAYER_1;
	// ���l�\��
	m_pNumber = CNumber::Create();
}

// ----------------------------------------
// �f�X�g���N�^����
// ----------------------------------------
CBullet::~CBullet()
{
}

// ----------------------------------------
// ����������
// ----------------------------------------
void CBullet::Init(void)
{
	// �T�C�Y�ݒ�
	CScene_TWO::SetSize(D3DXVECTOR2(BULLETSIZE, BULLETSIZE));
	// ������
	CScene_TWO::Init();
	// ���݂̈ʒu�ݒ�
	CScene_TWO::SetPosition(m_pos);
	// �e�N�X�`���[�ݒ�
	CScene_TWO::BindTexture(m_pTex[m_type]);
}

// ----------------------------------------
// �I������
// ----------------------------------------
void CBullet::Uninit(void)
{
	// �ԍ�
	delete m_pNumber;
	m_pNumber = NULL;

  	CScene_TWO::Uninit();
}

// ----------------------------------------
// �X�V����
// ----------------------------------------
void CBullet::Update(void)
{
	// ���݂̈ʒu�ݒ�
	CScene_TWO::SetPosition(m_pos);
	// �X�V
	CScene_TWO::Update();

	// ��Ԃɂ����
	if(m_state == STATE_OUT)			Goout();	// ���o��
	else if (m_state == STATE_PLAYER)	Player();	// �v���C���[�ҋ@��
	else if(m_state == STATE_BOX)		Box();		// �{�b�N�X�ҋ@��
	else								DownAnim();	// �������A�j���[�V����
}													

// ----------------------------------------
// �{�b�N�X�ҋ@��
// ----------------------------------------
void CBullet::Box(void)
{
}

// ----------------------------------------
// �v���C���[�ҋ@��
// ----------------------------------------
void CBullet::Player(void)
{
	// �ړ��J�E���g��20�����̏ꍇ
	if (m_nCntMove < BULLET_CNTMOVE)
	{
		m_pos.x += (m_posgoal.x - m_pos.x) / BULLET_CNTMOVE;	// �ڕW�n�_�ɋ߂Â�
		m_nCntMove++;								// �ړ��J�E���g�A�b�v
	}
	// �ړ��J�E���g��20�̏ꍇ
	else if (m_nCntMove == BULLET_CNTMOVE)
	{
		CPlayer * pPlayer = NULL;	// �v���C���[
		// ���擾
		pPlayer = (CPlayer*)CScene::GetActor(ACTOR_PLAYER, LAYER_ACTOR, GetIdScene(ACTOR_PLAYER, m_PlayerId));	// �v���C���[
		pPlayer->ShotState(false);																	// �e�ł��Ă��邩�̏��
		m_pos = m_posgoal;																			// �ڕW�n�_�ɑ��
	}
}

// ----------------------------------------
// ���łɑł������̏ꍇ
// ----------------------------------------
void CBullet::Goout(void)
{
	// �ړ�
	m_pos.x += m_move.x;
	m_pos.y += m_move.y;
	/* �����蔻�� */
	Collision();
	// �폜����
	Delete();
}

// ----------------------------------------
// ������A�j���[�V����
// ----------------------------------------
void CBullet::DownAnim(void)
{
	// �ړ���
	m_move.y += float(rand() % 10) / 10;
	// �ʒu�X�V
	m_pos.y += m_move.y;
	if (m_pos.y > 700.0f)
	{
		// �����[�X
		CScene::Release();
	}
}

// ----------------------------------------
// �����蔻��
// ----------------------------------------
void CBullet::Collision(void)
{
	// �ϐ��錾
	CBullet * pBullet;	// �e
	CFrame * pFrame;	// �t���[��
	CArray * pArray;	// �z��
	CPlayer * pPlayer;	// �v���C���[
	CBox * pBox;		// ��

	// ���擾
	pPlayer = (CPlayer*)CScene::GetActor(ACTOR_PLAYER, LAYER_ACTOR, GetIdScene(ACTOR_PLAYER, m_PlayerId));	// �v���C���[
	pFrame = (CFrame*)CScene::GetActor(ACTOR_FRAME, LAYER_BG, GetIdScene(ACTOR_FRAME, m_PlayerId));			// �t���[��
	pArray = (CArray *)GetActor(ACTOR_ARRAY, LAYER_ARRAY, GetIdScene(ACTOR_ARRAY,m_PlayerId));				// �z��
	pBox = (CBox *)GetActor(ACTOR_BOX, LAYER_BG, GetIdScene(ACTOR_BOX, m_PlayerId));						// ��

	/* �����蔻��̏��� */
	// �o�u���Q���
	for (int nCnt = 0; nCnt < SCENE_MAX; nCnt++)
	{
		// �ړ���Ԃł����
		if (m_bMove)
		{
			// ���擾
			pBullet = (CBullet*)CScene::GetActor(ACTOR_BULLET, LAYER_BULLET, nCnt);	// �e
			// �e�̏�񂪂���� 
			if (pBullet != NULL && pBullet != this)
			{
				if (pBullet->GetPlayerId() == m_PlayerId)
				{
					// ���o��Ԃł����
					if (pBullet->GetState() == STATE_OUT)
					{
						// �~�̓����蔻��
						if (CManager::GetCalculation()->Collision_Circle(m_pos, BULLET_COLLISIONSIZE, pBullet->m_pos, BULLET_COLLISIONSIZE))
						{
							// �k���`�F�b�N
							if (pArray != NULL)
							{
								// �߂��ʒu����Ԃ�
								m_pos = pArray->AroundPosCorrection(
									m_pos,					// �ʒu���
									pBullet->m_nLine,		// ���������e�̍s���
									pBullet->m_nColumn,		// ���������e�̗���
									&m_nLine,				// ������ɍs�����e�̍s���
									&m_nColumn);			// ������ɍs�����e�̗���
							}
							// �ړ��ʂ�����������
							m_move.x = 0;
							m_move.y = 0;
							// ������I���ɂ���
							m_bJudg = true;
							// �ړ���Ԃ��I�t�ɂ���
							m_bMove = false;
							// �v���C���[�Ɉʒu���ړ�
							BoxToPlayer();
							// �J�E���g�A�b�v
							m_nCntJudg++;

							// �i���o�[�\���f�o�b�N
							m_pNumber->SetNum(m_nCntJudg);
							m_pNumber->SetPos(m_pos, 20.0f);
							/* �����鏈�� */
							ConectDelete(this);
						}
					}
				}
			}
		}
	}

	// �k���`�F�b�N
	if (pFrame != NULL)
	{
		if (pFrame->GetPlayerId() == m_PlayerId)
		{
			// x�̌��E�l�𒴂����Ƃ�
			if (m_pos.x - CScene_TWO::GetSize().x / 2 <=
				pFrame->GetPos().x - pFrame->GetSize().x / 2 ||
				m_pos.x + CScene_TWO::GetSize().x / 2 >=
				pFrame->GetPos().x + pFrame->GetSize().x / 2)
			{
				m_move.x *= -1;
			}
			// y�̌��E�l�𒴂����Ƃ�
			if (m_pos.y - CScene_TWO::GetSize().y / 2 <
				pFrame->GetPos().y - pFrame->GetSize().y / 2)
			{
				// ���擾
				pArray = (CArray *)GetActor(ACTOR_ARRAY, LAYER_ARRAY, GetIdScene(ACTOR_ARRAY,m_PlayerId));	// �z��
				// �k���`�F�b�N
				if (pArray != NULL)
				{
					// �߂��ʒu����Ԃ�
					m_pos = pArray->TopPosCorrection(m_pos, 0, &m_nColumn);
					m_nLine = 0;
				}
				// �ړ��ʏ�����
				m_move.x = 0;
				m_move.y = 0;
				// �ړ����OFF
				m_bMove = false;
				// ������I���ɂ���
				m_bJudg = true;
				// �v���C���[�Ɉʒu���ړ�
				BoxToPlayer();

				/* ���̃A�j���[�V�����p�^�[�� */
				// �����̃A�j���[�V�����p�^�[���ݒ�
				pBox->SetVirticalAnim(1);
				// �����̃A�j���[�V�����p�^�[���ݒ�
				pBox->SetHorizonAnim(1);

				// �J�E���g�A�b�v
				m_nCntJudg++;
				// �i���o�[�\���f�o�b�N
				m_pNumber->SetNum(m_nCntJudg);
				m_pNumber->SetPos(m_pos, 20.0f);
				/* �����鏈�� */
				ConectDelete(this);
			}
		}
	}
}

// ----------------------------------------
// �A���폜����
// ----------------------------------------
void CBullet::ConectDelete(CBullet * bulletCom)	// ��r�������
{
	// �ϐ��錾
	int Direction;		// ���݂̕���
	CBullet * pBullet;	// �e

	// �����w��
	Direction = DIRECTION_NORTHWEST;	// �k�̕���

	while (1)
	{
		// �ׂ荇�����I�u�W�F�N�g�����邩�Ȃ���
		for (int nCnt = 0; nCnt < SCENE_MAX; nCnt++)
		{
			pBullet = (CBullet*)CScene::GetActor(CScene::ACTOR_BULLET, CScene::LAYER_BULLET, nCnt);	// ���擾
			if (pBullet != NULL)
			{
				if (pBullet->m_state == STATE_OUT)
				{
					if (pBullet->GetPlayerId() == bulletCom->GetPlayerId())
					{
						if (pBullet->m_type == m_type)
						{
							// �����A�h���X��������
							if (pBullet != bulletCom)
							{
								if (pBullet->m_bJudg == false)
								{
									// ���݂��Ă��邩�ǂ���
									if (Existence(pBullet, bulletCom, (DIRECTION)Direction))
									{
										// �����锻��
										pBullet->m_bJudg = true;
										// �J�E���g�A�b�v
										m_nCntJudg++;
										// �i���o�[�\���f�o�b�N
										pBullet->m_pNumber->SetNum(m_nCntJudg);
										pBullet->m_pNumber->SetPos(pBullet->GetPos(), 20.0f);
										/* �A���폜���� */
										ConectDelete(pBullet);
									}
								}
							}
						}
					}
				}
			}
		}
		// �����C���N�������g
		Direction++;

		if (Direction == DIRECTION_MAX)
		{
			return;
		}
	}
}

// ----------------------------------------
// �A����������
// ----------------------------------------
void CBullet::ConectDown(CBullet * bulletCom)
{
	// �ϐ��錾
	int Direction;		// ���݂̕���
	CBullet * pBullet;

	// �����w��
	Direction = DIRECTION_NORTHWEST;	// �k�̕���

	while (1)
	{
		// �ׂ荇�����I�u�W�F�N�g�����邩�Ȃ���
		for (int nCnt = 0; nCnt < SCENE_MAX; nCnt++)
		{
			pBullet = (CBullet*)CScene::GetActor(CScene::ACTOR_BULLET, CScene::LAYER_BULLET, nCnt);	// ���擾
			// �k���`�F�b�N
			if (pBullet != NULL)
			{
				if (pBullet->m_state == STATE_OUT)
				{
					if (pBullet->GetPlayerId() == bulletCom->GetPlayerId())
					{
						// �����A�h���X��������
						if (pBullet != bulletCom)
						{
							// ���肪false��������
							if (pBullet->m_bJudg == false)
							{
								// ���݂��Ă��邩�ǂ���
								if (Existence(pBullet, bulletCom, (DIRECTION)Direction))
								{
									// �����锻��
									pBullet->m_bJudg = true;
									/* �A���폜���� */
									ConectDown(pBullet);
								}
							}
						}
					}
				}
			}
		}
		// �����C���N�������g
		Direction++;

		if (Direction == DIRECTION_MAX)
		{
			return;
		}
	}
}

// ----------------------------------------
// �폜����
// ----------------------------------------
void CBullet::Delete(void)
{
	// �ϐ��錾
	CBullet * pBullet;	// ���
	CScore * pScore;	// �X�R�A
	bool bTopDelete = false;	// ��ɔz�u���Ă���A�������Ă��邩�ǂ���

	// �X�R�A���擾
	pScore = (CScore *)GetActor(ACTOR_SCORE, LAYER_UI, 0);
	
	/* �A���폜 */
	for (int nCnt = 0; nCnt < SCENE_MAX; nCnt++)
	{
		pBullet = (CBullet*)GetActor(ACTOR_BULLET, LAYER_BULLET, nCnt);	// ���擾
		if (pBullet != NULL)
		{
			if (pBullet->m_state == STATE_OUT)
			{
				if (pBullet->m_bJudg == true)
				{
					if (m_nCntJudg >= BULLET_CONNECT)
					{
						for (int nCntEffect = 0; nCntEffect < 20; nCntEffect++)
						{
							CEffect::Create(pBullet->GetPos(), CEffect::TYPE_BULLETDELETE, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
						}

						// ��ɔz�u���Ă���
						bTopDelete = true;
						pBullet->m_state = STATE_END;
						// �����[�X
						pBullet->Release();
						// �X�R�A�ݒ�
						pScore->SetScore(100);
					}
					else
					{
						pBullet->m_bJudg = false;
					}
				}
			}

		}
	}
	/* �A������ */
	if (bTopDelete)
	{
		// �폜
		for (int nCnt = 0; nCnt < SCENE_MAX; nCnt++)
		{
			pBullet = (CBullet*)GetActor(ACTOR_BULLET, LAYER_BULLET, nCnt);	// ���擾
			if (pBullet != NULL)
			{
				if (pBullet->m_PlayerId == m_PlayerId)
				{
					if (pBullet->m_state == STATE_OUT)
					{
						if (pBullet->m_nLine == 0)
						{
							// �Ȃ����Ă����Ԃ̎�
							if (pBullet->m_bJudg == false)
							{
								// �Ȃ����Ă�����
								pBullet->m_bJudg = true;
								// �A����������
								ConectDown(pBullet);
							}
						}
					}
				}
			}
		}
		/* �A���폜 */
		for (int nCnt = 0; nCnt < SCENE_MAX; nCnt++)
		{
			pBullet = (CBullet*)GetActor(ACTOR_BULLET, LAYER_BULLET, nCnt);	// ���擾
			if (pBullet != NULL)
			{
				if (pBullet->m_PlayerId == m_PlayerId)
				{
					if (pBullet->m_state == STATE_OUT)
					{
						// �Ȃ����Ă��Ȃ���Ԃ̎�
						if (pBullet->m_bJudg == false)
						{
							pBullet->m_state = STATE_END;
							/*
							// �����[�X
							pBullet->Release();
							*/
							// �X�R�A�ݒ�
							pScore->SetScore(100);
							// �J�E���g�A�b�v
							m_nCntJudg++;
						}
					}
				}
			}
		}
	}
	// ���v�|�C���g
	if (m_nCntJudg >= BULLET_CONNECT)
	{
		pScore->SetScore(500 * m_nCntJudg);
	}
	// �Ȃ���������������
	m_nCntJudg = 0;
}

// ----------------------------------------
// ������v���C���[�֏���
// ----------------------------------------
void CBullet::BoxToPlayer(void)
{
	// �ϐ��錾
	D3DXVECTOR3 Playerpos;
	D3DXVECTOR3 Boxpos;
	// ���擾
	CBox *pBox;
	CPlayer * pPlayer;	// �v���C���[

	// ���擾
	pBox = (CBox *)GetActor(ACTOR_BOX, LAYER_BG, GetIdScene(ACTOR_BOX, m_PlayerId));						// ��
	Boxpos = pBox->GetPos();																	// �ʒu
	pPlayer = (CPlayer*)CScene::GetActor(ACTOR_PLAYER, LAYER_ACTOR, GetIdScene(ACTOR_PLAYER,m_PlayerId));	// �v���C���[

	// ���݂̈ʒu���擾
	Playerpos = pPlayer->GetPos();

	for (int nCnt = 0; nCnt < SCENE_MAX; nCnt++)
	{
		// ���擾
		CBullet *pBullet = (CBullet *)CScene::GetActor(CScene::ACTOR_BULLET, CScene::LAYER_BULLET, nCnt);	// �e
		// �k���`�F�b�N
		if (pBullet != NULL)
		{
			if (pBullet->GetPlayerId() == m_PlayerId)
			{
				// ������v���C���[��
				if (pBullet->GetState() == CBullet::STATE_BOX)
				{
					pBullet->SetPosGoal(Playerpos);				// �ʒu�ݒ�
					pBullet->SetState(CBullet::STATE_PLAYER);	// ��Ԑݒ�
				}
			}
		}
	}
	// �e����
	CBullet::Create(m_PlayerId,Boxpos, (CBullet::TYPE)(rand() % (int)CBullet::TYPE_MAX));
}

// ----------------------------------------
// ���݂��邩���Ȃ�������
// ----------------------------------------
bool CBullet::Existence(CBullet * bulletOri, CBullet * bulletCom, DIRECTION direction)
{
	// �ϐ��錾
	bool bExist = false;	// ���݂��Ă��邩���Ȃ���

	/* �����ɉ����� */
	switch (direction)
	{
		// �k��
	case DIRECTION_NORTHWEST:
		// �񂪋�����
		if (bulletCom->m_nLine % 2 == 0)
		{
			// �z��ԍ����ꏏ��������
			if (bulletOri->m_nLine == bulletCom->m_nLine - 1 &&
				bulletOri->m_nColumn == bulletCom->m_nColumn - 1)
			{
				// ���݂��Ă���
				bExist = true;
			}
		}
		// �񂪊��
		else
		{
			// �z��ԍ����ꏏ��������
			if (bulletOri->m_nLine == bulletCom->m_nLine - 1 &&
				bulletOri->m_nColumn == bulletCom->m_nColumn)
			{
				// ���݂��Ă���
				bExist = true;
			}
		}
		break;
		// �k��
	case DIRECTION_NORTHEAST:
		// �񂪋�����
		if (bulletCom->m_nLine % 2 == 0)
		{
			// �z��ԍ����ꏏ��������
			if (bulletOri->m_nLine == bulletCom->m_nLine - 1 &&
				bulletOri->m_nColumn == bulletCom->m_nColumn)
			{
				// ���݂��Ă���
				bExist = true;
			}
		}
		// �񂪊��
		else
		{
			// �z��ԍ����ꏏ��������
			if (bulletOri->m_nLine == bulletCom->m_nLine - 1 &&
				bulletOri->m_nColumn == bulletCom->m_nColumn + 1)
			{
				// ���݂��Ă���
				bExist = true;
			}
		}
		break;
		// ��
	case DIRECTION_EAST:
		// �z��ԍ����ꏏ��������
		if (bulletOri->m_nLine == bulletCom->m_nLine &&
			bulletOri->m_nColumn == bulletCom->m_nColumn + 1)
		{
			// ���݂��Ă���
 			bExist = true;
		}
		
		break;
		// �쓌
	case DIRECTION_SOUTHEAST:
		// �񂪋�����
		if (bulletCom->m_nLine % 2 == 0)
		{
			// �z��ԍ����ꏏ��������
			if (bulletOri->m_nLine == bulletCom->m_nLine + 1 &&
				bulletOri->m_nColumn == bulletCom->m_nColumn)
			{
				// ���݂��Ă���
				bExist = true;
			}
		}
		// �񂪊��
		else
		{
			// �z��ԍ����ꏏ��������
			if (bulletOri->m_nLine == bulletCom->m_nLine + 1 &&
				bulletOri->m_nColumn == bulletCom->m_nColumn + 1)
			{
				// ���݂��Ă���
				bExist = true;
			}
		}
		break;
		// �쐼
	case DIRECTION_SOUTHWEST:
		// �񂪋�����
		if (bulletCom->m_nLine % 2 == 0)
		{
			// �z��ԍ����ꏏ��������
			if (bulletOri->m_nLine == bulletCom->m_nLine + 1 &&
				bulletOri->m_nColumn == bulletCom->m_nColumn - 1)
			{
				// ���݂��Ă���
				bExist = true;
			}
		}
		// �񂪊��
		else
		{
			// �z��ԍ����ꏏ��������
			if (bulletOri->m_nLine == bulletCom->m_nLine + 1 &&
				bulletOri->m_nColumn == bulletCom->m_nColumn)
			{
				// ���݂��Ă���
				bExist = true;
			}
		}
		break;
		// ��
	case DIRECTION_WEST:
		// �z��ԍ����ꏏ��������
		if (bulletOri->m_nLine == bulletCom->m_nLine &&
			bulletOri->m_nColumn == bulletCom->m_nColumn - 1)
		{
			// ���݂��Ă���
			bExist = true;
		}
		break;

	default:
		break;
	}
	return bExist;
}

// ----------------------------------------
// �`�揈��
// ----------------------------------------
void CBullet::Draw(void)
{
	CScene_TWO::Draw();
	m_pNumber->Draw();
}

// ----------------------------------------
// �ʒu���ݒ菈��
// ----------------------------------------
void CBullet::SetPos(D3DXVECTOR3 pos)
{
	m_pos = pos;
	m_posgoal = pos;
}

// ----------------------------------------
// �ڕW�n�_�̐ݒ菈��
// ----------------------------------------
void CBullet::SetPosGoal(D3DXVECTOR3 posgoal)
{
	m_posgoal = posgoal;
	m_posdif = (m_posgoal - m_pos) / 2 + m_pos;
}

// ----------------------------------------
// ��Ԑݒ菈��
// ----------------------------------------
void CBullet::SetRot(float frot)
{
	// �ړ��ʂ̐ݒ�
	m_move.x *= sinf(frot);
	m_move.y *= cosf(frot);
}

// ----------------------------------------
// ��Ԑݒ菈��
// ----------------------------------------
void CBullet::SetState(STATE state)
{
	// ��Ԑݒ�
	m_state = state;
	// �ړ���Ԃ�
	m_bMove = true;
}

// ----------------------------------------
// �v���C���[�ԍ��擾����
// ----------------------------------------
CManager::PLAYER CBullet::GetPlayerId(void)
{
	return m_PlayerId;
}

// ----------------------------------------
// ��Ԏ擾����
// ----------------------------------------
CBullet::STATE CBullet::GetState(void)
{
	return m_state;
}

// ----------------------------------------
// �ʒu���擾����
// ----------------------------------------
D3DXVECTOR3 CBullet::GetPos(void)
{
	return m_pos;
}

// ----------------------------------------
// �A�h���X�ňʒu���擾����
// ----------------------------------------
D3DXVECTOR3 * CBullet::Getpos(void)
{
	return &m_pos;
}

// ----------------------------------------
// �A�h���X�ňړ��ʏ��擾����
// ----------------------------------------
D3DXVECTOR3 * CBullet::GetMove(void)
{
	return &m_move;
}

// ----------------------------------------
// �ǂݍ��ݏ���
// ----------------------------------------
HRESULT CBullet::Load(void)
{
	// �ϐ��錾
	LPDIRECT3DDEVICE9 pDevice =					// �f�o�C�X�̎擾
		CManager::GetRenderer()->GetDevice();
	char cTex[CBullet::TYPE_MAX][128] =			// �e�N�X�`���[��
	{
		"data/TEXTURE/Bullet/Bee.png",
		"data/TEXTURE/Bullet/Cat.png",
		"data/TEXTURE/Bullet/Fish.png",
		"data/TEXTURE/Bullet/Hawk.png",
		"data/TEXTURE/Bullet/Snake.png",
	};

	// �e�N�X�`���[�ݒ�
	for (int nCnt = 0; nCnt < CBullet::TYPE_MAX; nCnt++)
	{
		D3DXCreateTextureFromFile(pDevice, cTex[nCnt], &m_pTex[nCnt]);
	}

	return S_OK;
}

// ----------------------------------------
// �ǂݍ��񂾏���j��
// ----------------------------------------
void CBullet::UnLoad(void)
{
	for (int nCnt = 0; nCnt < CBullet::TYPE_MAX; nCnt++)
	{
		// �e�N�X�`���̊J��
		if (m_pTex[nCnt] != NULL)
		{
			m_pTex[nCnt]->Release();
			m_pTex[nCnt] = NULL;
		}
	}
}

// ----------------------------------------
// �쐬����
// ----------------------------------------
CBullet * CBullet::Create(CManager::PLAYER PlayerId, D3DXVECTOR3 pos, TYPE type)
{
	// �ϐ��錾
	CBullet * pBullet;		// �e2D�N���X
	// �������̐���(����->��{�N���X,��->�h���N���X)
	pBullet = new CBullet();
	// ���݂̈ʒu
	pBullet->m_pos = pos;
	pBullet->m_posgoal = pos;
	// �^�C�v�̐ݒ�
	pBullet->m_type = type;
	// �v���C���[�ԍ�
	pBullet->m_PlayerId = PlayerId;
	// ����������
	pBullet->Init();
	// ���������I�u�W�F�N�g��Ԃ�
	return pBullet;
}