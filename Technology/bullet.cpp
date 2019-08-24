// ----------------------------------------
//
// 弾処理の説明[bullet.cpp]
// Author : Koki Nishiyama
//
// ----------------------------------------

// ----------------------------------------
//
// インクルードファイル
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
// マクロ定義
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
// グローバル変数
//
// ----------------------------------------

// ----------------------------------------
//
// 静的変数宣言
//
// ----------------------------------------
LPDIRECT3DTEXTURE9 CBullet::m_pTex[CBullet::TYPE_MAX] = {};
int CBullet::m_nCntJudg = 0;

// ----------------------------------------
// コンストラクタ処理
// ----------------------------------------
CBullet::CBullet() : CScene_TWO::CScene_TWO(ACTOR_BULLET,LAYER_BULLET)
{
	/* 変数の初期化 */
	// 位置
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	// 目的位置
	m_posgoal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	// 移動量
	m_move = D3DXVECTOR3(BULLET_MOVE, BULLET_MOVE, 0.0f);
	// タイプ
	m_type = TYPE_BEE;
	// 打った後か前か
	m_state = STATE_BOX;
	// 回転量
	m_fRot = D3DX_PI;
	// 体力
	m_nLife = 50;
	// 行番号
	m_nLine = 7;
	// 列番号
	m_nColumn = 9;
	// 移動カウント
	m_nCntJudg = 0;
	// つながっている判定
	m_bJudg = false;
	// 落ちてる判定
	m_bDown = false;
	// 移動しているかどうか
	m_bMove = false;
	// プレイヤー番号
	m_PlayerId = CManager::PLAYER_1;
	// 数値表示
	m_pNumber = CNumber::Create();
}

// ----------------------------------------
// デストラクタ処理
// ----------------------------------------
CBullet::~CBullet()
{
}

// ----------------------------------------
// 初期化処理
// ----------------------------------------
void CBullet::Init(void)
{
	// サイズ設定
	CScene_TWO::SetSize(D3DXVECTOR2(BULLETSIZE, BULLETSIZE));
	// 初期化
	CScene_TWO::Init();
	// 現在の位置設定
	CScene_TWO::SetPosition(m_pos);
	// テクスチャー設定
	CScene_TWO::BindTexture(m_pTex[m_type]);
}

// ----------------------------------------
// 終了処理
// ----------------------------------------
void CBullet::Uninit(void)
{
	// 番号
	delete m_pNumber;
	m_pNumber = NULL;

  	CScene_TWO::Uninit();
}

// ----------------------------------------
// 更新処理
// ----------------------------------------
void CBullet::Update(void)
{
	// 現在の位置設定
	CScene_TWO::SetPosition(m_pos);
	// 更新
	CScene_TWO::Update();

	// 状態によって
	if(m_state == STATE_OUT)			Goout();	// 放出後
	else if (m_state == STATE_PLAYER)	Player();	// プレイヤー待機中
	else if(m_state == STATE_BOX)		Box();		// ボックス待機中
	else								DownAnim();	// 落ちたアニメーション
}													

// ----------------------------------------
// ボックス待機中
// ----------------------------------------
void CBullet::Box(void)
{
}

// ----------------------------------------
// プレイヤー待機中
// ----------------------------------------
void CBullet::Player(void)
{
	// 移動カウントが20未満の場合
	if (m_nCntMove < BULLET_CNTMOVE)
	{
		m_pos.x += (m_posgoal.x - m_pos.x) / BULLET_CNTMOVE;	// 目標地点に近づく
		m_nCntMove++;								// 移動カウントアップ
	}
	// 移動カウントが20の場合
	else if (m_nCntMove == BULLET_CNTMOVE)
	{
		CPlayer * pPlayer = NULL;	// プレイヤー
		// 情報取得
		pPlayer = (CPlayer*)CScene::GetActor(ACTOR_PLAYER, LAYER_ACTOR, GetIdScene(ACTOR_PLAYER, m_PlayerId));	// プレイヤー
		pPlayer->ShotState(false);																	// 弾打っているかの状態
		m_pos = m_posgoal;																			// 目標地点に代入
	}
}

// ----------------------------------------
// すでに打った時の場合
// ----------------------------------------
void CBullet::Goout(void)
{
	// 移動
	m_pos.x += m_move.x;
	m_pos.y += m_move.y;
	/* 当たり判定 */
	Collision();
	// 削除処理
	Delete();
}

// ----------------------------------------
// 落ちるアニメーション
// ----------------------------------------
void CBullet::DownAnim(void)
{
	// 移動量
	m_move.y += float(rand() % 10) / 10;
	// 位置更新
	m_pos.y += m_move.y;
	if (m_pos.y > 700.0f)
	{
		// リリース
		CScene::Release();
	}
}

// ----------------------------------------
// 当たり判定
// ----------------------------------------
void CBullet::Collision(void)
{
	// 変数宣言
	CBullet * pBullet;	// 弾
	CFrame * pFrame;	// フレーム
	CArray * pArray;	// 配列
	CPlayer * pPlayer;	// プレイヤー
	CBox * pBox;		// 箱

	// 情報取得
	pPlayer = (CPlayer*)CScene::GetActor(ACTOR_PLAYER, LAYER_ACTOR, GetIdScene(ACTOR_PLAYER, m_PlayerId));	// プレイヤー
	pFrame = (CFrame*)CScene::GetActor(ACTOR_FRAME, LAYER_BG, GetIdScene(ACTOR_FRAME, m_PlayerId));			// フレーム
	pArray = (CArray *)GetActor(ACTOR_ARRAY, LAYER_ARRAY, GetIdScene(ACTOR_ARRAY,m_PlayerId));				// 配列
	pBox = (CBox *)GetActor(ACTOR_BOX, LAYER_BG, GetIdScene(ACTOR_BOX, m_PlayerId));						// 箱

	/* 当たり判定の処理 */
	// バブル２回目
	for (int nCnt = 0; nCnt < SCENE_MAX; nCnt++)
	{
		// 移動状態であれば
		if (m_bMove)
		{
			// 情報取得
			pBullet = (CBullet*)CScene::GetActor(ACTOR_BULLET, LAYER_BULLET, nCnt);	// 弾
			// 弾の情報があれば 
			if (pBullet != NULL && pBullet != this)
			{
				if (pBullet->GetPlayerId() == m_PlayerId)
				{
					// 放出状態であれば
					if (pBullet->GetState() == STATE_OUT)
					{
						// 円の当たり判定
						if (CManager::GetCalculation()->Collision_Circle(m_pos, BULLET_COLLISIONSIZE, pBullet->m_pos, BULLET_COLLISIONSIZE))
						{
							// ヌルチェック
							if (pArray != NULL)
							{
								// 近い位置情報を返す
								m_pos = pArray->AroundPosCorrection(
									m_pos,					// 位置情報
									pBullet->m_nLine,		// 当たった弾の行情報
									pBullet->m_nColumn,		// 当たった弾の列情報
									&m_nLine,				// 当たりに行った弾の行情報
									&m_nColumn);			// 当たりに行った弾の列情報
							}
							// 移動量を初期化する
							m_move.x = 0;
							m_move.y = 0;
							// 判定をオンにする
							m_bJudg = true;
							// 移動状態をオフにする
							m_bMove = false;
							// プレイヤーに位置を移動
							BoxToPlayer();
							// カウントアップ
							m_nCntJudg++;

							// ナンバー表示デバック
							m_pNumber->SetNum(m_nCntJudg);
							m_pNumber->SetPos(m_pos, 20.0f);
							/* 消える処理 */
							ConectDelete(this);
						}
					}
				}
			}
		}
	}

	// ヌルチェック
	if (pFrame != NULL)
	{
		if (pFrame->GetPlayerId() == m_PlayerId)
		{
			// xの限界値を超えたとき
			if (m_pos.x - CScene_TWO::GetSize().x / 2 <=
				pFrame->GetPos().x - pFrame->GetSize().x / 2 ||
				m_pos.x + CScene_TWO::GetSize().x / 2 >=
				pFrame->GetPos().x + pFrame->GetSize().x / 2)
			{
				m_move.x *= -1;
			}
			// yの限界値を超えたとき
			if (m_pos.y - CScene_TWO::GetSize().y / 2 <
				pFrame->GetPos().y - pFrame->GetSize().y / 2)
			{
				// 情報取得
				pArray = (CArray *)GetActor(ACTOR_ARRAY, LAYER_ARRAY, GetIdScene(ACTOR_ARRAY,m_PlayerId));	// 配列
				// ヌルチェック
				if (pArray != NULL)
				{
					// 近い位置情報を返す
					m_pos = pArray->TopPosCorrection(m_pos, 0, &m_nColumn);
					m_nLine = 0;
				}
				// 移動量初期化
				m_move.x = 0;
				m_move.y = 0;
				// 移動状態OFF
				m_bMove = false;
				// 判定をオンにする
				m_bJudg = true;
				// プレイヤーに位置を移動
				BoxToPlayer();

				/* 箱のアニメーションパターン */
				// 垂直のアニメーションパターン設定
				pBox->SetVirticalAnim(1);
				// 水平のアニメーションパターン設定
				pBox->SetHorizonAnim(1);

				// カウントアップ
				m_nCntJudg++;
				// ナンバー表示デバック
				m_pNumber->SetNum(m_nCntJudg);
				m_pNumber->SetPos(m_pos, 20.0f);
				/* 消える処理 */
				ConectDelete(this);
			}
		}
	}
}

// ----------------------------------------
// 連鎖削除処理
// ----------------------------------------
void CBullet::ConectDelete(CBullet * bulletCom)	// 比較するもの
{
	// 変数宣言
	int Direction;		// 現在の方向
	CBullet * pBullet;	// 弾

	// 方向指定
	Direction = DIRECTION_NORTHWEST;	// 北の方向

	while (1)
	{
		// 隣り合ったオブジェクトがあるかないか
		for (int nCnt = 0; nCnt < SCENE_MAX; nCnt++)
		{
			pBullet = (CBullet*)CScene::GetActor(CScene::ACTOR_BULLET, CScene::LAYER_BULLET, nCnt);	// 情報取得
			if (pBullet != NULL)
			{
				if (pBullet->m_state == STATE_OUT)
				{
					if (pBullet->GetPlayerId() == bulletCom->GetPlayerId())
					{
						if (pBullet->m_type == m_type)
						{
							// 同じアドレスだったら
							if (pBullet != bulletCom)
							{
								if (pBullet->m_bJudg == false)
								{
									// 存在しているかどうか
									if (Existence(pBullet, bulletCom, (DIRECTION)Direction))
									{
										// 消える判定
										pBullet->m_bJudg = true;
										// カウントアップ
										m_nCntJudg++;
										// ナンバー表示デバック
										pBullet->m_pNumber->SetNum(m_nCntJudg);
										pBullet->m_pNumber->SetPos(pBullet->GetPos(), 20.0f);
										/* 連鎖削除処理 */
										ConectDelete(pBullet);
									}
								}
							}
						}
					}
				}
			}
		}
		// 方向インクリメント
		Direction++;

		if (Direction == DIRECTION_MAX)
		{
			return;
		}
	}
}

// ----------------------------------------
// 連鎖落ち処理
// ----------------------------------------
void CBullet::ConectDown(CBullet * bulletCom)
{
	// 変数宣言
	int Direction;		// 現在の方向
	CBullet * pBullet;

	// 方向指定
	Direction = DIRECTION_NORTHWEST;	// 北の方向

	while (1)
	{
		// 隣り合ったオブジェクトがあるかないか
		for (int nCnt = 0; nCnt < SCENE_MAX; nCnt++)
		{
			pBullet = (CBullet*)CScene::GetActor(CScene::ACTOR_BULLET, CScene::LAYER_BULLET, nCnt);	// 情報取得
			// ヌルチェック
			if (pBullet != NULL)
			{
				if (pBullet->m_state == STATE_OUT)
				{
					if (pBullet->GetPlayerId() == bulletCom->GetPlayerId())
					{
						// 同じアドレスだったら
						if (pBullet != bulletCom)
						{
							// 判定がfalseだったら
							if (pBullet->m_bJudg == false)
							{
								// 存在しているかどうか
								if (Existence(pBullet, bulletCom, (DIRECTION)Direction))
								{
									// 消える判定
									pBullet->m_bJudg = true;
									/* 連鎖削除処理 */
									ConectDown(pBullet);
								}
							}
						}
					}
				}
			}
		}
		// 方向インクリメント
		Direction++;

		if (Direction == DIRECTION_MAX)
		{
			return;
		}
	}
}

// ----------------------------------------
// 削除処理
// ----------------------------------------
void CBullet::Delete(void)
{
	// 変数宣言
	CBullet * pBullet;	// 情報
	CScore * pScore;	// スコア
	bool bTopDelete = false;	// 上に配置している泡が消えているかどうか

	// スコア情報取得
	pScore = (CScore *)GetActor(ACTOR_SCORE, LAYER_UI, 0);
	
	/* 連鎖削除 */
	for (int nCnt = 0; nCnt < SCENE_MAX; nCnt++)
	{
		pBullet = (CBullet*)GetActor(ACTOR_BULLET, LAYER_BULLET, nCnt);	// 情報取得
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

						// 上に配置している
						bTopDelete = true;
						pBullet->m_state = STATE_END;
						// リリース
						pBullet->Release();
						// スコア設定
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
	/* 連鎖落ち */
	if (bTopDelete)
	{
		// 削除
		for (int nCnt = 0; nCnt < SCENE_MAX; nCnt++)
		{
			pBullet = (CBullet*)GetActor(ACTOR_BULLET, LAYER_BULLET, nCnt);	// 情報取得
			if (pBullet != NULL)
			{
				if (pBullet->m_PlayerId == m_PlayerId)
				{
					if (pBullet->m_state == STATE_OUT)
					{
						if (pBullet->m_nLine == 0)
						{
							// つながっている状態の時
							if (pBullet->m_bJudg == false)
							{
								// つながっている状態
								pBullet->m_bJudg = true;
								// 連鎖落ち処理
								ConectDown(pBullet);
							}
						}
					}
				}
			}
		}
		/* 連鎖削除 */
		for (int nCnt = 0; nCnt < SCENE_MAX; nCnt++)
		{
			pBullet = (CBullet*)GetActor(ACTOR_BULLET, LAYER_BULLET, nCnt);	// 情報取得
			if (pBullet != NULL)
			{
				if (pBullet->m_PlayerId == m_PlayerId)
				{
					if (pBullet->m_state == STATE_OUT)
					{
						// つながっていない状態の時
						if (pBullet->m_bJudg == false)
						{
							pBullet->m_state = STATE_END;
							/*
							// リリース
							pBullet->Release();
							*/
							// スコア設定
							pScore->SetScore(100);
							// カウントアップ
							m_nCntJudg++;
						}
					}
				}
			}
		}
	}
	// 合計ポイント
	if (m_nCntJudg >= BULLET_CONNECT)
	{
		pScore->SetScore(500 * m_nCntJudg);
	}
	// つながった個数を初期化
	m_nCntJudg = 0;
}

// ----------------------------------------
// 箱からプレイヤーへ処理
// ----------------------------------------
void CBullet::BoxToPlayer(void)
{
	// 変数宣言
	D3DXVECTOR3 Playerpos;
	D3DXVECTOR3 Boxpos;
	// 情報取得
	CBox *pBox;
	CPlayer * pPlayer;	// プレイヤー

	// 情報取得
	pBox = (CBox *)GetActor(ACTOR_BOX, LAYER_BG, GetIdScene(ACTOR_BOX, m_PlayerId));						// 箱
	Boxpos = pBox->GetPos();																	// 位置
	pPlayer = (CPlayer*)CScene::GetActor(ACTOR_PLAYER, LAYER_ACTOR, GetIdScene(ACTOR_PLAYER,m_PlayerId));	// プレイヤー

	// 現在の位置情報取得
	Playerpos = pPlayer->GetPos();

	for (int nCnt = 0; nCnt < SCENE_MAX; nCnt++)
	{
		// 情報取得
		CBullet *pBullet = (CBullet *)CScene::GetActor(CScene::ACTOR_BULLET, CScene::LAYER_BULLET, nCnt);	// 弾
		// ヌルチェック
		if (pBullet != NULL)
		{
			if (pBullet->GetPlayerId() == m_PlayerId)
			{
				// 箱からプレイヤーへ
				if (pBullet->GetState() == CBullet::STATE_BOX)
				{
					pBullet->SetPosGoal(Playerpos);				// 位置設定
					pBullet->SetState(CBullet::STATE_PLAYER);	// 状態設定
				}
			}
		}
	}
	// 弾生成
	CBullet::Create(m_PlayerId,Boxpos, (CBullet::TYPE)(rand() % (int)CBullet::TYPE_MAX));
}

// ----------------------------------------
// 存在するかしないか処理
// ----------------------------------------
bool CBullet::Existence(CBullet * bulletOri, CBullet * bulletCom, DIRECTION direction)
{
	// 変数宣言
	bool bExist = false;	// 存在しているかいないか

	/* 方向に応じて */
	switch (direction)
	{
		// 北西
	case DIRECTION_NORTHWEST:
		// 列が偶数列
		if (bulletCom->m_nLine % 2 == 0)
		{
			// 配列番号が一緒だったら
			if (bulletOri->m_nLine == bulletCom->m_nLine - 1 &&
				bulletOri->m_nColumn == bulletCom->m_nColumn - 1)
			{
				// 存在している
				bExist = true;
			}
		}
		// 列が奇数列
		else
		{
			// 配列番号が一緒だったら
			if (bulletOri->m_nLine == bulletCom->m_nLine - 1 &&
				bulletOri->m_nColumn == bulletCom->m_nColumn)
			{
				// 存在している
				bExist = true;
			}
		}
		break;
		// 北東
	case DIRECTION_NORTHEAST:
		// 列が偶数列
		if (bulletCom->m_nLine % 2 == 0)
		{
			// 配列番号が一緒だったら
			if (bulletOri->m_nLine == bulletCom->m_nLine - 1 &&
				bulletOri->m_nColumn == bulletCom->m_nColumn)
			{
				// 存在している
				bExist = true;
			}
		}
		// 列が奇数列
		else
		{
			// 配列番号が一緒だったら
			if (bulletOri->m_nLine == bulletCom->m_nLine - 1 &&
				bulletOri->m_nColumn == bulletCom->m_nColumn + 1)
			{
				// 存在している
				bExist = true;
			}
		}
		break;
		// 東
	case DIRECTION_EAST:
		// 配列番号が一緒だったら
		if (bulletOri->m_nLine == bulletCom->m_nLine &&
			bulletOri->m_nColumn == bulletCom->m_nColumn + 1)
		{
			// 存在している
 			bExist = true;
		}
		
		break;
		// 南東
	case DIRECTION_SOUTHEAST:
		// 列が偶数列
		if (bulletCom->m_nLine % 2 == 0)
		{
			// 配列番号が一緒だったら
			if (bulletOri->m_nLine == bulletCom->m_nLine + 1 &&
				bulletOri->m_nColumn == bulletCom->m_nColumn)
			{
				// 存在している
				bExist = true;
			}
		}
		// 列が奇数列
		else
		{
			// 配列番号が一緒だったら
			if (bulletOri->m_nLine == bulletCom->m_nLine + 1 &&
				bulletOri->m_nColumn == bulletCom->m_nColumn + 1)
			{
				// 存在している
				bExist = true;
			}
		}
		break;
		// 南西
	case DIRECTION_SOUTHWEST:
		// 列が偶数列
		if (bulletCom->m_nLine % 2 == 0)
		{
			// 配列番号が一緒だったら
			if (bulletOri->m_nLine == bulletCom->m_nLine + 1 &&
				bulletOri->m_nColumn == bulletCom->m_nColumn - 1)
			{
				// 存在している
				bExist = true;
			}
		}
		// 列が奇数列
		else
		{
			// 配列番号が一緒だったら
			if (bulletOri->m_nLine == bulletCom->m_nLine + 1 &&
				bulletOri->m_nColumn == bulletCom->m_nColumn)
			{
				// 存在している
				bExist = true;
			}
		}
		break;
		// 西
	case DIRECTION_WEST:
		// 配列番号が一緒だったら
		if (bulletOri->m_nLine == bulletCom->m_nLine &&
			bulletOri->m_nColumn == bulletCom->m_nColumn - 1)
		{
			// 存在している
			bExist = true;
		}
		break;

	default:
		break;
	}
	return bExist;
}

// ----------------------------------------
// 描画処理
// ----------------------------------------
void CBullet::Draw(void)
{
	CScene_TWO::Draw();
	m_pNumber->Draw();
}

// ----------------------------------------
// 位置情報設定処理
// ----------------------------------------
void CBullet::SetPos(D3DXVECTOR3 pos)
{
	m_pos = pos;
	m_posgoal = pos;
}

// ----------------------------------------
// 目標地点の設定処理
// ----------------------------------------
void CBullet::SetPosGoal(D3DXVECTOR3 posgoal)
{
	m_posgoal = posgoal;
	m_posdif = (m_posgoal - m_pos) / 2 + m_pos;
}

// ----------------------------------------
// 状態設定処理
// ----------------------------------------
void CBullet::SetRot(float frot)
{
	// 移動量の設定
	m_move.x *= sinf(frot);
	m_move.y *= cosf(frot);
}

// ----------------------------------------
// 状態設定処理
// ----------------------------------------
void CBullet::SetState(STATE state)
{
	// 状態設定
	m_state = state;
	// 移動状態に
	m_bMove = true;
}

// ----------------------------------------
// プレイヤー番号取得処理
// ----------------------------------------
CManager::PLAYER CBullet::GetPlayerId(void)
{
	return m_PlayerId;
}

// ----------------------------------------
// 状態取得処理
// ----------------------------------------
CBullet::STATE CBullet::GetState(void)
{
	return m_state;
}

// ----------------------------------------
// 位置情報取得処理
// ----------------------------------------
D3DXVECTOR3 CBullet::GetPos(void)
{
	return m_pos;
}

// ----------------------------------------
// アドレスで位置情報取得処理
// ----------------------------------------
D3DXVECTOR3 * CBullet::Getpos(void)
{
	return &m_pos;
}

// ----------------------------------------
// アドレスで移動量情報取得処理
// ----------------------------------------
D3DXVECTOR3 * CBullet::GetMove(void)
{
	return &m_move;
}

// ----------------------------------------
// 読み込み処理
// ----------------------------------------
HRESULT CBullet::Load(void)
{
	// 変数宣言
	LPDIRECT3DDEVICE9 pDevice =					// デバイスの取得
		CManager::GetRenderer()->GetDevice();
	char cTex[CBullet::TYPE_MAX][128] =			// テクスチャー名
	{
		"data/TEXTURE/Bullet/Bee.png",
		"data/TEXTURE/Bullet/Cat.png",
		"data/TEXTURE/Bullet/Fish.png",
		"data/TEXTURE/Bullet/Hawk.png",
		"data/TEXTURE/Bullet/Snake.png",
	};

	// テクスチャー設定
	for (int nCnt = 0; nCnt < CBullet::TYPE_MAX; nCnt++)
	{
		D3DXCreateTextureFromFile(pDevice, cTex[nCnt], &m_pTex[nCnt]);
	}

	return S_OK;
}

// ----------------------------------------
// 読み込んだ情報を破棄
// ----------------------------------------
void CBullet::UnLoad(void)
{
	for (int nCnt = 0; nCnt < CBullet::TYPE_MAX; nCnt++)
	{
		// テクスチャの開放
		if (m_pTex[nCnt] != NULL)
		{
			m_pTex[nCnt]->Release();
			m_pTex[nCnt] = NULL;
		}
	}
}

// ----------------------------------------
// 作成処理
// ----------------------------------------
CBullet * CBullet::Create(CManager::PLAYER PlayerId, D3DXVECTOR3 pos, TYPE type)
{
	// 変数宣言
	CBullet * pBullet;		// 弾2Dクラス
	// メモリの生成(初め->基本クラス,後->派生クラス)
	pBullet = new CBullet();
	// 現在の位置
	pBullet->m_pos = pos;
	pBullet->m_posgoal = pos;
	// タイプの設定
	pBullet->m_type = type;
	// プレイヤー番号
	pBullet->m_PlayerId = PlayerId;
	// 初期化処理
	pBullet->Init();
	// 生成したオブジェクトを返す
	return pBullet;
}