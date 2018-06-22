//----------------------------------------------------------------------//
// シンプルスクロールバー・パネル										//
//																		//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <algorithm>
#include "usr_scrpanel.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
//リストボックスに関連付け
//---------------------------------------------------------------------------
UsrScrollPanel::UsrScrollPanel(TPanel *pp, TListBox *lp, int flag)
{
	ParentPanel   = pp;
	AssoListBox   = lp;
	AssoStrGrid   = NULL;
	AssoScrollBar = NULL;
	Flag = flag;

	InitializePanel();
}
//---------------------------------------------------------------------------
//グリッドに関連付け
//---------------------------------------------------------------------------
UsrScrollPanel::UsrScrollPanel(TPanel *pp, TStringGrid *gp, int flag)
{
	ParentPanel   = pp;
	AssoStrGrid   = gp;
	AssoListBox   = NULL;
	AssoScrollBar = NULL;
	Flag = flag;

	InitializePanel();
}
//---------------------------------------------------------------------------
//垂直スクロールバーに関連付け
//---------------------------------------------------------------------------
UsrScrollPanel::UsrScrollPanel(TPanel *pp, TScrollBar *sp, int flag)
{
	ParentPanel   = pp;
	AssoScrollBar = sp;
	AssoListBox   = NULL;
	AssoStrGrid   = NULL;
	Flag = flag;

	InitializePanel();
}

//---------------------------------------------------------------------------
UsrScrollPanel::~UsrScrollPanel()
{
	ScrPanelV->WindowProc  = org_ScrPanelVWndProc;
	if (ScrPanelH && org_ScrPanelHWndProc) 	ScrPanelH->WindowProc	= org_ScrPanelHWndProc;

	if (org_ParentPanelWndProc) 			ParentPanel->WindowProc = org_ParentPanelWndProc;
	if (AssoListBox && org_AssoListWndProc) AssoListBox->WindowProc = org_AssoListWndProc;
	if (AssoStrGrid && org_AssoGridWndProc) AssoStrGrid->WindowProc = org_AssoGridWndProc;
}

//---------------------------------------------------------------------------
void __fastcall UsrScrollPanel::InitializePanel()
{
	KnobImgBuffV = NULL;
	KnobImgBuffH = NULL;

	OnRButtonUp  = NULL;

	KnobWidth = ::GetSystemMetrics(SM_CXVSCROLL);

	ListCsrVisible = false;

	//垂直スクロールバー
	ScrPanelV = new TPanel(ParentPanel);
	ScrPanelV->Parent	  = ParentPanel;
	ScrPanelV->BevelOuter = bvNone;
	ScrPanelV->Align	  = alRight;
	ScrPanelV->Width	  = KnobWidth;

	org_ScrPanelVWndProc  = ScrPanelV->WindowProc;
	ScrPanelV->WindowProc = ScrPanelVWndProc;

	ScrPaintBoxV = new TPaintBox(ParentPanel);
	ScrPaintBoxV->Parent	  = ScrPanelV;
	ScrPaintBoxV->Align 	  = alClient;
	ScrPaintBoxV->OnPaint	  = ScrPaintBoxPaint;
	ScrPaintBoxV->OnMouseDown = ScrPaintBoxMouseDown;
	ScrPaintBoxV->OnMouseMove = ScrPaintBoxMouseMove;
	ScrPaintBoxV->OnMouseUp   = ScrPaintBoxMouseUp;
	ScrPaintBoxV->Tag		  = 0;

	//水平スクロール
	ScrPanelH	 = NULL;
	ScrPaintBoxH = NULL;
	org_ScrPanelHWndProc = NULL;
	if (Flag & USCRPNL_FLAG_HS) {
		ScrPanelH = new TPanel(ParentPanel);
		ScrPanelH->Parent	  = ParentPanel;
		ScrPanelH->BevelOuter = bvNone;
		ScrPanelH->Height	  = KnobWidth;
		ScrPanelH->Width	  = ParentPanel->ClientWidth;
		ScrPanelH->Left		  = 0;
		ScrPanelH->Top		  = ParentPanel->ClientHeight - ScrPanelH->Height;
		TAnchors anchor;
		anchor << akLeft << akRight << akBottom;
		ScrPanelH->Anchors	  = anchor;

		org_ScrPanelHWndProc  = ScrPanelH->WindowProc;
		ScrPanelH->WindowProc = ScrPanelHWndProc;

		ScrPaintBoxH = new TPaintBox(ParentPanel);
		ScrPaintBoxH->Parent	  = ScrPanelH;
		ScrPaintBoxH->Align 	  = alClient;
		ScrPaintBoxH->OnPaint	  = ScrPaintBoxPaint;
		ScrPaintBoxH->OnMouseDown = ScrPaintBoxMouseDown;
		ScrPaintBoxH->OnMouseMove = ScrPaintBoxMouseMove;
		ScrPaintBoxH->OnMouseUp   = ScrPaintBoxMouseUp;
		ScrPaintBoxH->Tag		  = 1;
	}

	ScrKnobRectV = Rect(0, 0, 0, 0);
	ScrKnobMaxY  = 0;
	ScrCatchYp	 = 0;

	ScrKnobRectH = Rect(0, 0, 0, 0);
	ScrKnobMaxX	 = 0;
	ScrCatchXp	 = 0;

	ScrCatchKnob = false;

	Color		 = clAppWorkSpace;
	KnobColor	 = clBtnFace;
	KnobBdrColor = clBtnShadow;
	Visible 	 = false;
	VisibleV	 = VisibleH = false;

	org_ParentPanelWndProc = NULL;
	if (Flag & USCRPNL_FLAG_P_WP) {
		org_ParentPanelWndProc	= ParentPanel->WindowProc;
		ParentPanel->WindowProc = ParentPanelWndProc;
	}

	org_AssoListWndProc = NULL;
	if (AssoListBox && (Flag & USCRPNL_FLAG_L_WP)) {
		org_AssoListWndProc 	= AssoListBox->WindowProc;
		AssoListBox->WindowProc = AssoListWndProc;
	}

	org_AssoGridWndProc = NULL;
	if (AssoStrGrid && (Flag & USCRPNL_FLAG_G_WP)) {
		org_AssoGridWndProc 	= AssoStrGrid->WindowProc;
		AssoStrGrid->WindowProc = AssoGridWndProc;
	}
}

//---------------------------------------------------------------------------
void __fastcall UsrScrollPanel::SetVisible(bool Value)
{
	FVisible = Value;
	if (!FVisible) {
		ScrPanelV->Visible = VisibleV = false;
		if (ScrPanelH) ScrPanelH->Visible = VisibleH = false;
	}
}

//---------------------------------------------------------------------------
//グリッドの列幅の合計
//---------------------------------------------------------------------------
int __fastcall UsrScrollPanel::get_GridTotalWidth()
{
	if (!AssoStrGrid) return 1;

	int wd = 0;
	for (int i=0; i<AssoStrGrid->ColCount; i++) wd += AssoStrGrid->ColWidths[i];
	return wd;
}
//---------------------------------------------------------------------------
//水平スクロール位置からグリッドの先頭列を設定
//---------------------------------------------------------------------------
void __fastcall UsrScrollPanel::set_GridLefCol(int pos)
{
	if (!AssoStrGrid) return;

	if (pos>=ScrKnobMaxX)
		AssoStrGrid->LeftCol = AssoStrGrid->ColCount - 1;
	else {
		int scr_p = (int)(1.0 * pos / (ParentPanel->ClientWidth - KnobWidth) * get_GridTotalWidth());
		for (int i=0,w=0; i<AssoStrGrid->ColCount; i++,w+=AssoStrGrid->ColWidths[i]) {
			if (scr_p<=w) { AssoStrGrid->LeftCol = i; break; }
		}
	}
}

//---------------------------------------------------------------------------
//グリッドの行高の合計
//---------------------------------------------------------------------------
int __fastcall UsrScrollPanel::get_GridTotalHeight()
{
	if (!AssoStrGrid) return 1;

	int hi = 0;
	for (int i=0; i<AssoStrGrid->RowCount; i++) hi += AssoStrGrid->RowHeights[i];
	return hi;
}
//---------------------------------------------------------------------------
//垂直スクロール位置からグリッドの先頭行を設定
//---------------------------------------------------------------------------
void __fastcall UsrScrollPanel::set_GridTopRow(int pos)
{
	if (!AssoStrGrid) return;

	int scr_p = (int)(1.0 * pos / AssoStrGrid->ClientHeight * get_GridTotalHeight());
	for (int i=0,h=0; i<AssoStrGrid->RowCount; i++,h+=AssoStrGrid->RowHeights[i]) {
		if (scr_p<=h) { AssoStrGrid->TopRow = i; break; }
	}
}

//---------------------------------------------------------------------------
//リストボックスの水平スクロール位置を取得しノブ位置に変換
//---------------------------------------------------------------------------
int __fastcall UsrScrollPanel::GetKnobPosH()
{
	int xp = 0;
	if (AssoListBox && AssoListBox->ScrollWidth>0) {
		SCROLLINFO si;
		::ZeroMemory(&si, sizeof(SCROLLINFO));
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask  = SIF_TRACKPOS;
		if (::GetScrollInfo(AssoListBox->Handle, SB_HORZ, &si))
			xp = (int)(1.0 * si.nTrackPos / AssoListBox->ScrollWidth * (ParentPanel->ClientWidth - KnobWidth));
	}
	else if (AssoStrGrid && AssoStrGrid->ColCount>0) {
		int x0 = 0;
		for (int i=0; i<AssoStrGrid->LeftCol; i++) x0 += AssoStrGrid->ColWidths[i];
		xp = (int)(1.0 * x0 / get_GridTotalWidth() * (ParentPanel->ClientWidth - KnobWidth));
	}
	return xp;
}

//---------------------------------------------------------------------------
//表示、ノブサイズ等の更新
//---------------------------------------------------------------------------
void __fastcall UsrScrollPanel::UpdateKnob()
{
	if (Visible) {
		ScrKnobRectV = Rect(0, 0, 0, 0);
		ScrKnobRectH = Rect(0, 0, 0, 0);
		ScrPanelV->Width	= KnobWidth;
		ScrPaintBoxV->Width = ScrPanelV->ClientWidth;

		if (ScrPanelH && ScrPaintBoxH) {
			ScrPanelH->Width	 = ParentPanel->ClientWidth;
			ScrPanelH->Height	 = KnobWidth;
			ScrPanelH->Top		 = ParentPanel->ClientHeight - ScrPanelH->Height;
			ScrPaintBoxH->Height = ScrPanelH->ClientHeight;
		}

		//リストボックス
		if (AssoListBox) {
			TListBox *lp = AssoListBox;
			//垂直スクロールバー
			if (lp->ItemHeight>0 && lp->Count>0) {
				int lst_hi = lp->ClientHeight;
				ScrPage = lst_hi/lp->ItemHeight;
				if (lp->Count>ScrPage) {
					double r = 1.0 * ScrPage / lp->Count;
					ScrKnobRectV = Rect(0, 0, ScrPaintBoxV->Width, std::max((int)(lst_hi * r), KnobWidth));
					ScrKnobMaxY  = lst_hi - ScrKnobRectV.Height() - 1;
					int yp = std::min((int)(1.0 * lp->TopIndex / lp->Count * lst_hi), ScrKnobMaxY);
					ScrKnobRectV.Location = Point(0, yp);
					ScrPanelV->Visible = true;
					ScrPaintBoxV->Invalidate();
				}
				else ScrPanelV->Visible = false;
			}
			else ScrPanelV->Visible = false;

			//水平スクロールバー
			if (ScrPanelH && ScrPaintBoxH) {
				if (lp->ScrollWidth>lp->ClientWidth) {
					double r = 1.0 * lp->ClientWidth / lp->ScrollWidth;
					int hscr_wd  = ParentPanel->ClientWidth - KnobWidth;
					ScrKnobRectH = Rect(0, 0, std::max((int)(hscr_wd * r), KnobWidth - 2), ScrPaintBoxH->Height);
					ScrKnobMaxX  = hscr_wd - ScrKnobRectH.Width() - 1;
					ScrKnobRectH.Location = Point(GetKnobPosH(), 0);
					ScrPanelH->Visible = true;
					ScrPaintBoxH->Invalidate();
				}
				else ScrPanelH->Visible = false;

				//リストボックスの高さ調整
				lp->Height = ParentPanel->ClientHeight
								+ (ScrPanelH->Visible? (::GetSystemMetrics(SM_CYHSCROLL) - ScrPanelH->Height + 1) : 0);
			}
		}
		//グリッド
		else if (AssoStrGrid) {
			TStringGrid *gp = AssoStrGrid;
			//垂直スクロールバー
			if (gp->RowCount>0) {
				int grid_hi = gp->ClientHeight;
				ScrPage = gp->VisibleRowCount;
				if (gp->RowCount > ScrPage) {
					double r = 1.0 * grid_hi / get_GridTotalHeight();
					ScrKnobRectV = Rect(0, 0, ScrPaintBoxV->Width, std::max((int)(grid_hi * r), KnobWidth));
					ScrKnobMaxY  = grid_hi - ScrKnobRectV.Height() - 1;
					int yp = std::min((int)(1.0 * gp->TopRow / gp->RowCount * grid_hi), ScrKnobMaxY);
					ScrKnobRectV.Location = Point(0, yp);
					ScrPanelV->Visible = true;
					ScrPaintBoxV->Invalidate();
				}
				else ScrPanelV->Visible = false;
			}
			else ScrPanelV->Visible = false;

			//水平スクロールバー
			if (ScrPanelH && ScrPaintBoxH) {
				if (gp->ColCount>gp->VisibleColCount) {
					double r = 1.0 * gp->ClientWidth / get_GridTotalWidth();
					int hscr_wd  = ParentPanel->ClientWidth - KnobWidth;
					ScrKnobRectH = Rect(0, 0, std::max((int)(hscr_wd * r), KnobWidth - 2), ScrPaintBoxH->Height);
					ScrKnobMaxX  = hscr_wd - ScrKnobRectH.Width() - 1;
					ScrKnobRectH.Location = Point(GetKnobPosH(), 0);
					ScrPanelH->Visible = true;
					ScrPaintBoxH->Invalidate();
				}
				else ScrPanelH->Visible = false;

				//グリッドの高さ調整
				gp->Height = ParentPanel->ClientHeight
								+ (ScrPanelH->Visible? (::GetSystemMetrics(SM_CYHSCROLL) - ScrPanelH->Height + 1) : 0);
			}
		}
		//スクロールバー
		else if (AssoScrollBar) {
			TScrollBar *bp = AssoScrollBar;
			//垂直スクロールバー
			if (bp->Max>0) {
				ScrPage = bp->LargeChange;
				if (bp->Max>ScrPage) {
					int scr_hi = ScrPanelV->ClientHeight;
					double r = 1.0 * ScrPage / (bp->Max - bp->Min);
					ScrKnobRectV = Rect(0, 0, ScrPaintBoxV->Width, std::max((int)(scr_hi * r), KnobWidth));
					ScrKnobMaxY  = scr_hi - ScrKnobRectV.Height() - 1;
					int yp = std::min((int)(1.0 * (bp->Position - bp->Min) / (bp->Max - bp->Min) * scr_hi), ScrKnobMaxY);
					ScrKnobRectV.Location = Point(0, yp);
					ScrPanelV->Visible = true;
					ScrPaintBoxV->Invalidate();
				}
				else ScrPanelV->Visible = false;
			}
			else ScrPanelV->Visible = false;
		}
		else ScrPanelV->Visible = false;
	}
	else ScrPanelV->Visible = false;

	VisibleV = ScrPanelV->Visible;
	VisibleH = (ScrPanelH && ScrPanelH->Visible);
}

//---------------------------------------------------------------------------
void __fastcall UsrScrollPanel::ParentPanelWndProc(TMessage &msg)
{
	//背景のちらつき防止
	if (msg.Msg==WM_ERASEBKGND) { msg.Result = 1; return; }

	org_ParentPanelWndProc(msg);

	if (msg.Msg==WM_SIZE) UpdateKnob();
}
//---------------------------------------------------------------------------
void __fastcall UsrScrollPanel::AssoListWndProc(TMessage &msg)
{
	//ノブ表示を同期
	if (msg.Msg==WM_PAINT && !ScrCatchKnob) Repaint();

	org_AssoListWndProc(msg);
}
//---------------------------------------------------------------------------
void __fastcall UsrScrollPanel::AssoGridWndProc(TMessage &msg)
{
	//ノブ表示を同期
	if (msg.Msg==WM_PAINT && !ScrCatchKnob) Repaint();

	org_AssoGridWndProc(msg);
}
//---------------------------------------------------------------------------
void __fastcall UsrScrollPanel::ScrPanelVWndProc(TMessage &msg)
{
	//背景のちらつき防止
	if (msg.Msg==WM_ERASEBKGND) { msg.Result = 1; return; }

	org_ScrPanelVWndProc(msg);

	if (org_ParentPanelWndProc==NULL && msg.Msg==WM_SIZE) UpdateKnob();
}
//---------------------------------------------------------------------------
void __fastcall UsrScrollPanel::ScrPanelHWndProc(TMessage &msg)
{
	//背景のちらつき防止
	if (msg.Msg==WM_ERASEBKGND) { msg.Result = 1; return; }

	org_ScrPanelHWndProc(msg);

	if (org_ParentPanelWndProc==NULL && msg.Msg==WM_SIZE) UpdateKnob();
}

//---------------------------------------------------------------------------
//描画
//---------------------------------------------------------------------------
void __fastcall UsrScrollPanel::ScrPaintBoxPaint(TObject *Sender)
{
	if (ScrPanelV->Visible || (ScrPanelH && ScrPanelH->Visible)) {
		TPaintBox *pp = (TPaintBox *)Sender;
		TCanvas *cv = pp->Canvas;

		//背景
		cv->Brush->Color = Color;
		cv->FillRect(pp->ClientRect);

		//つまみ
		TRect rc = (pp->Tag==1)? ScrKnobRectH : ScrKnobRectV;
		Graphics::TBitmap *bp = (pp->Tag==1)? KnobImgBuffH : KnobImgBuffV;
		if (bp && !bp->Empty) {
			InflateRect(rc, -1, -1);
			if (pp->Tag==1) rc.Right += 1; else rc.Bottom += 1;
			cv->StretchDraw(rc, bp);
		}
		else if (KnobBdrColor==clNone) {
			InflateRect(rc, -1, -1);
			if (pp->Tag==1) rc.Right += 1; else rc.Bottom += 1;
			cv->Brush->Style = bsSolid;
			cv->Brush->Color = KnobColor;
			cv->FillRect(rc);
		}
		else {
			if (pp->Tag==1) {
				InflateRect(rc, 0, -2);  rc.Left += 1;
			}
			else {
				InflateRect(rc, -2, 0);  rc.Top += 1;
			}
			cv->Brush->Style = bsSolid;
			cv->Brush->Color = KnobColor;
			cv->FillRect(rc);
			//輪郭
			cv->Pen->Style = psSolid;
			cv->Pen->Color = KnobBdrColor;
			cv->MoveTo(rc.Left,		rc.Top - 1);	cv->LineTo(rc.Right, 	rc.Top - 1);
			cv->MoveTo(rc.Left - 1, rc.Top);		cv->LineTo(rc.Left - 1,	rc.Bottom);
			cv->MoveTo(rc.Left, 	rc.Bottom);		cv->LineTo(rc.Right,	rc.Bottom);
			cv->MoveTo(rc.Right, 	rc.Top);		cv->LineTo(rc.Right,	rc.Bottom);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall UsrScrollPanel::Repaint()
{
	//垂直スクロールバー
	if (ScrPanelV->Visible) {
		//リストボックス
		if (AssoListBox && AssoListBox->Count>0) {
			if (ScrKnobRectV.Height()>0) {
				int yp = std::min((int)(1.0 * AssoListBox->TopIndex / AssoListBox->Count * AssoListBox->ClientHeight), ScrKnobMaxY);
				ScrKnobRectV.Location = Point(0, yp);
			}
		}
		//グリッド
		else if (AssoStrGrid && AssoStrGrid->RowCount>0) {
			if (ScrKnobRectV.Height()>0) {
				int yp = std::min((int)(1.0 * AssoStrGrid->TopRow / AssoStrGrid->RowCount * AssoStrGrid->ClientHeight), ScrKnobMaxY);
				ScrKnobRectV.Location = Point(0, yp);
			}
		}
		//スクロールバー
		else if (AssoScrollBar && AssoScrollBar->Max>0) {
			double r = 1.0 * (AssoScrollBar->Position - AssoScrollBar->Min) / (AssoScrollBar->Max - AssoScrollBar->Min);
			int yp = std::min((int)(r * AssoScrollBar->Height), ScrKnobMaxY);
			ScrKnobRectV.Location = Point(0, yp);
		}
		ScrPaintBoxV->Repaint();
	}

	//水平スクロールバー
	if (ScrPanelH && ScrPaintBoxH && ScrPanelH->Visible && ScrKnobRectH.Width()>0) {
		if ((AssoListBox && AssoListBox->Count>0) || (AssoStrGrid && AssoStrGrid->RowCount>0)) {
			ScrKnobRectH.Location = Point(GetKnobPosH(), 0);
			ScrPaintBoxH->Repaint();
		}
	}
}

//---------------------------------------------------------------------------
//マウス操作
//---------------------------------------------------------------------------
void __fastcall UsrScrollPanel::ScrPaintBoxMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (Button==mbLeft) {
		TPaintBox *pp = (TPaintBox *)Sender;
		TRect rc = (pp->Tag==1)? ScrKnobRectH : ScrKnobRectV;
		ScrCatchKnob = rc.PtInRect(Point(X, Y));
		//ノブ移動開始
		if (ScrCatchKnob) {
			if (pp->Tag==0)
				ScrCatchYp = Y - rc.Top;
			else
				ScrCatchXp = X - rc.Left;
		}
		//前後ページ
		else {
			//リストボックス
			if (AssoListBox) {
				TListBox *lp = AssoListBox;
				//垂直スクロールバー
				if (pp->Tag==0) {
					if (Y<rc.Top)
						lp->TopIndex = std::max(lp->TopIndex - ScrPage, 0);
					else if (Y>rc.Bottom)
						lp->TopIndex = lp->TopIndex + ScrPage;
				}
				//水平スクロールバー
				else {
					if (X<rc.Left) {
						lp->Perform(WM_HSCROLL, MAKEWPARAM(SB_PAGELEFT,  0), 0);
						lp->Perform(WM_HSCROLL, MAKEWPARAM(SB_ENDSCROLL, 0), 0);
					}
					else if (X>rc.Right) {
						lp->Perform(WM_HSCROLL, MAKEWPARAM(SB_PAGERIGHT, 0), 0);
						lp->Perform(WM_HSCROLL, MAKEWPARAM(SB_ENDSCROLL, 0), 0);
					}
				}
			}
			//グリッド
			else if (AssoStrGrid) {
				TStringGrid *gp = AssoStrGrid;
				//垂直スクロールバー
				if (pp->Tag==0) {
					if (Y<rc.Top)
						gp->TopRow = std::max(gp->TopRow - ScrPage, 0);
					else if (Y>rc.Bottom)
						gp->TopRow = gp->TopRow + ScrPage;
				}
				//水平スクロールバー
				else {
					if (X<rc.Left) {
						gp->Perform(WM_HSCROLL, MAKEWPARAM(SB_PAGELEFT,  0), 0);
						gp->Perform(WM_HSCROLL, MAKEWPARAM(SB_ENDSCROLL, 0), 0);
					}
					else if (X>rc.Right) {
						gp->Perform(WM_HSCROLL, MAKEWPARAM(SB_PAGERIGHT, 0), 0);
						gp->Perform(WM_HSCROLL, MAKEWPARAM(SB_ENDSCROLL, 0), 0);
					}
				}
			}
			//スクロールバー
			else if (AssoScrollBar) {
				if (Y<rc.Top)
					AssoScrollBar->Position = std::max(AssoScrollBar->Position - ScrPage, 0);
				else if (Y>rc.Bottom)
					AssoScrollBar->Position = AssoScrollBar->Position + ScrPage;
			}
			Repaint();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall UsrScrollPanel::ScrPaintBoxMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
	if (ScrCatchKnob) {
		TPaintBox *pp = (TPaintBox *)Sender;
		//垂直スクロールバー
		if (pp->Tag==0) {
			int yp = std::min(std::max(Y - ScrCatchYp, 0), ScrKnobMaxY);
			ScrKnobRectV.Location = Point(ScrKnobRectV.Left, yp);
			pp->Repaint();
			//リストボックス
			if (AssoListBox) {
				if (yp==ScrKnobMaxY)
					AssoListBox->TopIndex = AssoListBox->Count - 1;
				else
					AssoListBox->TopIndex = 1.0 * yp / AssoListBox->ClientHeight * AssoListBox->Count;

				if (Flag & (USCRPNL_FLAG_FL|USCRPNL_FLAG_GL)) AssoListBox->Perform(WM_NYANFI_USCROLL, 0, 0);

				//カーソルを常に可視領域に
				if (ListCsrVisible) {
					if (AssoListBox->ItemIndex < AssoListBox->TopIndex)
						AssoListBox->ItemIndex = AssoListBox->TopIndex;
					else {
						int idx_btm = AssoListBox->TopIndex + AssoListBox->ClientHeight/AssoListBox->ItemHeight - 1;
						if (AssoListBox->ItemIndex > idx_btm) AssoListBox->ItemIndex = idx_btm;
					}
				}
			}
			//グリッド
			else if (AssoStrGrid) {
				set_GridTopRow(yp);
			}
			//スクロールバー
			else if (AssoScrollBar) {
				int rng = (AssoScrollBar->Max - AssoScrollBar->Min);
				if (yp==ScrKnobMaxY)
					AssoScrollBar->Position = std::max(rng - ScrPage, AssoScrollBar->Min);
				else
					AssoScrollBar->Position = 1.0 * yp / AssoScrollBar->Height * rng;
			}
		}
		//水平スクロールバー
		else {
			if (ScrPaintBoxH && (AssoListBox || AssoStrGrid) && ScrKnobMaxX>0) {
				int xp = std::min(std::max(X - ScrCatchXp, 0), ScrKnobMaxX);
				ScrKnobRectH.Location = Point(xp, 0);
				ScrPaintBoxH->Repaint();

				int hscr_wd  = ParentPanel->ClientWidth - KnobWidth;
				if (hscr_wd>0) {
					//リストボックス
					if (AssoListBox) {
						int scr_p = (int)(1.0 * xp / hscr_wd * AssoListBox->ScrollWidth);
						AssoListBox->Perform(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, scr_p), 0);
						AssoListBox->Perform(WM_HSCROLL, MAKEWPARAM(SB_ENDSCROLL, 0), 0);
					}
					//グリッド
					else if (AssoStrGrid && AssoStrGrid->ColCount>0) {
						set_GridLefCol(xp);
					}
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall UsrScrollPanel::ScrPaintBoxMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
	TPaintBox *pp = (TPaintBox *)Sender;
	ScrCatchKnob = false;

	//右クリックイベント
	if (Button==mbRight && OnRButtonUp && pp->Tag==0) OnRButtonUp(Sender, Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
