sudo ./killbuild
cd build
sudo bash ../VcompLabAluno.bash
sudo chmod -R ugo+rw *
sudo chmod -R ugo+rw ../alunoout
make
make lexdiff

# quando quiser comparar os detalhes descomente a linha abaixo
meld ../alunodetail/ ../detailonlylex/

# batcat ../alunodetail/*lex*diff