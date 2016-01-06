剛体シミュ的ナニカ Yururu (0.6.0 SHIORI "TenderTentacle" KANNA)
64bit windows限定
Windows7でしか確認してません
α未満
完成するかも怪しい

・実装済みエレメント
- Core
- Chip
- Rudder
- Trim
- Wheel
- Frame
- RudderF
- TrimF
その他に読み込めるけど見栄が不完全だったり
挙動がおかしかったり

・未実装、不具合
- angle, name, power,brake,spring,damper 以外の全てパラメータ
- 空気抵抗
- ネットワーク(ガワのみで中身はありません)
- 初期読み込み位置に傾斜があると喰われる
- Land読み込み時にモデルの位置が再計算されていない
- Reload時にモデルの運動がリセットされていない
その他数えきれないくらい多くのモノ


・ver M.ASAKURA(0.5.0)からの修正点
- Waterの浮力、抵抗計算
- 違うディレクトリから読み込んだ後、
リソースが読めなくなる問題の修正
- 初期読み込み時の位置計算
- spring, damperの物理演算
