#include <fstream>
#include <cstdlib>

int main(){
	std::ofstream ofs("generateFriends.sql");
	ofs << "begin;" << std::endl;
	for(int i=1;i<=10000;++i){
		int j;
		while((j=(std::rand()%10000+1))==i){

		}
		ofs << "insert into friends(id1,id2) " << "values(" << i << "," << j << ");" << std::endl;
		ofs << "insert into friends(id1,id2) " << "values(" << j << "," << i << ");" << std::endl;
	}
	int i,j;
	for(int k=0;k<50000;){
		i=std::rand()%10000+1;
		j=std::rand()%10000+1;
		if(i==j) continue;
		ofs << "insert into friends(id1,id2) " << "values(" << i << "," << j << ");" << std::endl;
		ofs << "insert into friends(id1,id2) " << "values(" << j << "," << i << ");" << std::endl;

		++k;
	}
	ofs << "commit;" << std::endl;
}
