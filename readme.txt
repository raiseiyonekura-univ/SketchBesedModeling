コンパイルコマンド：
g++ -std=c++20 -I /usr/local/include/eigen-3.4.0　 -I /usr/local/include/libigl/include Main.cpp -o main EstimateHeight.cpp InputFunction.cpp MeshDeformation.cpp MeshStructure.cpp Triangulation.cpp

実行方法:
./main "filename".svg
モデル化したいsvgファイルをsketchフォルダ内に保存し、上記のコマンドを入力することで実行ができます

解説
svg形式により保存されたスケッチをボリュームメッシュにするプログラムです。入力のスケッチの各パーツは一筆書きである必要があります

