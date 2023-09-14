makefileの使い方

makefileを実行して実行ファイル（rho.o）を得たい場合
コマンドプロンプトで
　make
を実行

実行ファイルを削除する場合
 make clean
を実行



２個目のmakefile_solveを実行して実行ファイル(answer.o)を得たい場合
 make -f makefile_solve
を実行

実行ファイルを削除する場合
 make clean　-f makefile_solve
を実行
