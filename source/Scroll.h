
#define DIRECTION_UP	0
#define DIRECTION_DOWN	1
#define DIRECTION_LEFT	2
#define DIRECTION_RIGHT	3

typedef struct ScrollData{
	private:
		long hpos;//Horizontal scroll value
		long vpos;//Vertical scroll value
		SCROLLINFO scr_info;
		long vScrollMax;
	public:
		void SetIniFile();
		BOOL InitScroll(void);//初期化
		void VertScrollProc(short mode, long scroll);//スクロールさせた時の処理
		void HorzScrollProc(short mode, long scroll);
		void WheelScrollProc(LPARAM lParam, WPARAM wParam); //ホイールを...
		void GetScrollPosition(long *hp,long *vp);
		void SetHorzScroll(long x);
		void AttachScroll(void);	//ウィンドウサイズ変更に応じてスクロールを修正
		void KeyScroll(int iDirection); //キー操作によるスクロール用
		void PrintHorzPosition(void);
		void ChangeVerticalRange(int WindowHeight = -1); //ウィンドウサイズに応じてスクロールバーRangeを変更
		void ChangeHorizontalRange(int range); //ウィンドウサイズに応じてスクロールバーRangeを変更
}SCROLLDATA;
extern SCROLLDATA scr_data;//スクロールデータ
extern long MAXHORZRANGE;
