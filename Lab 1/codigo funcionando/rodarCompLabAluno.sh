sudo ./killbuild
cd build
sudo bash ../compLabAluno.bash
sudo chmod -R ugo+rw *
sudo chmod -R ugo+rw ../alunoout
make
make lexdiff
make ddiff

# quando quiser comparar os detalhes descomente a linha abaixo
# meld ../alunodetail/ ../detail/

# batcat ../alunodetail/*lex*diff