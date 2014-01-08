#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<cstdlib>
#include<cctype>
#include<algorithm>
#include<vector>
#include<math.h>

#define NUMBER 500
#define PREDICT 100

#define MAX_TAGS 10 //Maximum tags per question
int cos_sim;
using namespace std;

struct learnings{
	int tag_in_title[MAX_TAGS];
	int body_count[MAX_TAGS];
};
typedef struct learnings learnt;

class question{
	long id;
	string title;
	string body;
	string tag_list;
	learnt l;
	
	int tag_number;
	
	public:
		void learn();
		void processTitle(string current_tag);
		void processBody(string current_tag);
		void analyzeCode();
				
		void setId(long l){
			id= l;
		}
		
		void setTitle(string s){
			title= s;
		}

		void setBody(string s){
			body= s;
		}

		void setTagList(string s){
			tag_list= s;			
		}

		string getTitle(){
			return title;
		}

		long getId(){
			return id;
		}

		string getBody(){
			return body;
		}

		string getTagList(){
			return tag_list;
		}

		learnt getLearnings(){
			return l;
		}

};

class que{

	string token;
 	int idf_value;

	public:
		void learn();
		void processTitle(string current_tag);
		void processBody(string current_tag);
		void analyzeCode();

		void setToken(string s){
			token= s;
		}
		string getToken(){
			return token;
		}

	
};
//tries to predict the language from the code in the question body
void question::analyzeCode(){

}

//removes HTML tags from the question body
void removeHtmlTags(){

}

void question::learn(){
	istringstream iss(tag_list);
	string token;

	analyzeCode();
	removeHtmlTags();	
	tag_number=0;
	while( getline(iss,token,' ') ){
		processTitle(token);
		processBody(token);
		tag_number++;
	}
}

//counts number of occurences of current_tag in question body
void question::processBody(string current_tag){
	int pos;
	int count=0;
	istringstream iss(current_tag);
	string i_tag;

	while( getline(iss,i_tag,'-') ){
		pos=-1;
		do{
			pos=body.find(i_tag,pos+1);
			if( pos!=string::npos && !isalnum(body[pos-1]) && !isalnum(body[pos+i_tag.size()]) )
				count++;
		}while(pos!=string::npos);
	}

	l.body_count[tag_number]=count;
}


//returns the index at which current tag occurs in question title, else returns string::npos
void question::processTitle(string current_tag){
	if( title.find(current_tag)==string::npos )
		l.tag_in_title[tag_number]=0;	
	else
		l.tag_in_title[tag_number]=1;
}

ios::pos_type searchFile(fstream &file,string token){
	file.clear();
	file.seekg(0,ios::beg);
	string line;
	string file_token;
	ios::pos_type current;
	
	while( file ){
		current=file.tellg();	
		getline(file,line);
		istringstream iss(line);
		getline(iss,file_token,' ');
		if( file_token==token )
			return current;
	}
	return -1;
}

/**** Class for training the program ******/

class train{
	fstream train_file;
	fstream tags_file;
	fstream vector_file;
	question q;

	public:
		
		train(char file_name[]){
			train_file.open( file_name,ios::in|ios::binary );
			tags_file.open( "tags_file.txt",ios::in|ios::out|ios::trunc|ios::binary );
			vector_file.open( "vector_file.txt",ios::in|ios::out|ios::trunc|ios::binary );
			if( !train_file || !tags_file ){
				cout<<"Cannot open file"<<endl;
				exit(1);
			}
		}
		
		void beginTraining();
		void writeLearnings();
		void getQuestion();	
};



void train::beginTraining(){
	getQuestion();  //neglect file headings
	
	while(train_file){
	//for(int i=0;i<NUMBER;i++){
		getQuestion();
		q.learn();
		writeLearnings();
	}
	vector_file<<cos_sim<<endl;
	
	train_file.close();
	tags_file.close();
	vector_file.close();	
}

void train::getQuestion(){
	string s,t;
	char ch;
	
	for(int i=0;i<4;i++){
	  getline(train_file,s,'"');
	  getline(train_file,s,'"');

	  if(i!=3){	
		train_file.get(ch);
		while(ch!=',' && train_file){
			getline(train_file,t,'"');
			s = s+t;
			train_file.get(ch);
		}
	  }	  
		
	  switch(i){
		case 0: q.setId(atol( s.c_str() )); 
		  		break;
		case 1: transform(s.begin(), s.end(), s.begin(), ::tolower);
		  		q.setTitle(s);
		  		break;
        case 2: transform(s.begin(), s.end(), s.begin(), ::tolower); 
			  	q.setBody(s); 
		  		break;
		case 3:	transform(s.begin(), s.end(), s.begin(), ::tolower); 
			  	q.setTagList(s); 
		  		break;
	  }
	}
}


//writes the counts in the tags_file
void train::writeLearnings(){
	istringstream iss(q.getTagList());
	string token;
	int index;
	
	
	learnt l= q.getLearnings();	

	ios::pos_type pos;
	index=0;
	while( getline(iss,token,' ') ){
		pos= ::searchFile(tags_file,token);

		if( pos==(ios::pos_type)-1 ){
			int tag_body;
			tag_body = 	l.tag_in_title[index] + l.body_count[index];		
		//insert new token in the tags_file
			tags_file.clear();
			tags_file.seekp(0,ios::end);
			tags_file.width(40);
			tags_file.setf(ios::left);
			tags_file<<token;
			tags_file.width(20);
			tags_file<<tag_body;
			//tags_file.width(20);
			//tags_file<<l.body_count[index];
			//tags_file.width(20);
			tags_file<<endl;
			vector_file.width(40);
			vector_file.setf(ios::left);
				
					
			if(tag_body == 0)
			{
				vector_file<<0;
			}
			else{
				vector_file<<(1 + log(tag_body));
				//cos_sim += (1+log(tag_body))*(1+log(tag_body));
			}
			vector_file.width(20);
			vector_file<<token<<endl;
		}
		else{
		//add findings of this token to previous entry of the token
			int in_title;
			int tag_occurrence;
			tags_file.clear();
			tags_file.seekg(pos,ios::beg);
			
			
			tags_file>>token>>in_title>>tag_occurrence;
			
			
			if(l.tag_in_title[index])
				in_title++;
			tag_occurrence+= l.body_count[index];
			tags_file.seekp(pos,ios::beg);
			tags_file.setf(ios::left);
			tags_file.width(40);
			tags_file<<token;
			tags_file.width(20);
			tags_file<<(in_title + tag_occurrence);
			tags_file<<endl;
			
		}
		
		index++;
	}
	//vector_file<<cos_sim<<endl;
}


int main( int argc, char* argv[] ){

	train t(argv[1]);
	t.beginTraining();
	
}

