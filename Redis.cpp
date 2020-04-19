#include<bits/stdc++.h>
#include <stdio.h>
#include <fstream>
#include<time.h>
#include<ctime>
using namespace std;
#define nil "$-1\r\n"
#define ok "OK"
#define sys_clean_time 180
class Redis{
	
	private:
		
		unordered_map<string, pair<string,long> >store; //hashmap of key-value pair where value stores key-value and time of insertion
		unordered_map<string, long>time_to_live;  //hashmap of key with value as time to live 
		unordered_map<string, set<pair<long,string> > >sorted_sets; //hashmap of all the ordered sets
		unordered_map<string, unordered_map<string,long> >member_score_maps; //hashmap of the {members,score} pair of a particular ordered set
		
		int last_clean_time=time(0); //last time when timm_to_clean function was called
	
	public:
		
		//creates necessary files if not present on User's PC
		static void createFile(string fname){
			ofstream file;
			file.open(fname.c_str());
			file.close();
		}
		
		//method to fill the hashmaps when the program first loads
		void initialize(){
			int c=this->readMemberScoreMaps("MemberScoreMaps");
			c=this->readSortedSets("SortedSets");
			c=this->readStore("Store");
			c=this->readTTL("time_to_live");
		}
		
		//dumping the data into file database when program ends so that data is not lost
		void dumpData(){
			int c=this->writeMemberScoreMaps("MemberScoreMaps");
			c=this->writeSortedSets("SortedSets");
			c=this->writeStore("Store");
			c=this->writeTimetolive("time_to_live");
		}
		
		//static because this method doesn't belong to a particular object. It behaves same for all the objects
		//this method converts string to integer
		static long toInt(string s){
			int l=s.length();
			long num=0;
			long sign=1;
			for(int i=0;i<l;i++){
				if(s[i]=='-'){
					sign=-1;continue;
				}
				num=num*10+(s[i]-'0');
			}
			return num*sign;
		}
		
		//writing from time_to_live hashmap to the file
		int writeTimetolive(string fname) {
			unordered_map<string, long> *m=&time_to_live;
        	int count = 0;
        	if (m->empty()){
                return 0;
            }

			//opening the file
        	FILE *fp = fopen(fname.c_str(), "w");
        	if (!fp){
                return -errno;
            }
            
			//reading from map and putting key-value into file separated by '=' sign
        	for(unordered_map<string, long>::iterator it = m->begin(); it != m->end(); it++) {
        		stringstream ss;
        		ss<<it->second;
        		string key=it->first;
        		string ttl=ss.str();
                fprintf(fp, "%s=%s\n", key.c_str(), ttl.c_str());
                count++;
        	}

        	fclose(fp);
        	return count;
		}
		
		//writing from Store hashmap to the file
		int writeStore(string fname) {
			unordered_map<string, pair<string,long> > *m=&store;
        	int count = 0;
        	if (m->empty()){
                return 0;
            }

        	FILE *fp = fopen(fname.c_str(), "w");
        	if (!fp){
                return -errno;
            }

			//putting entries into file as "key=value=insert_time"
        	for(unordered_map<string, pair<string,long> >::iterator it = m->begin(); it != m->end(); it++) {
        		stringstream ss;
        		ss<<it->second.second;
        		string key=it->first,value=it->second.first,insert_time=ss.str();
                fprintf(fp, "%s=%s=%s\n", key.c_str(), value.c_str(),insert_time.c_str());
                count++;
        	}

        	fclose(fp);
        	return count;
		}
		
		//writing from SortedSet hashmap to the file
		int writeSortedSets(string fname) {
			unordered_map<string, set<pair<long,string> > > *m=&sorted_sets;
        	int count = 0;
        	if (m->empty()){
                return 0;
            }

        	FILE *fp = fopen(fname.c_str(), "w");
        	if (!fp){
                return -errno;
            }

			//putting entries into file as "key=score=value=member"
        	for(unordered_map<string, set<pair<long,string> > >::iterator it = m->begin(); it != m->end(); it++) {
        		string key=it->first;
        		for(set<pair<long,string> >::iterator sit=(it->second).begin();sit!=(it->second).end();sit++){
					stringstream ss;
        			string score,member;
        			ss<<(*sit).first;
        			score=ss.str();
        			member=(*sit).second;
                	fprintf(fp, "%s=%s=%s\n", key.c_str(), score.c_str(),member.c_str());
                	count++;	
				}
        	}

        	fclose(fp);
        	return count;
		}
		
		//writing from member_score_maps hashmap to the file
		int writeMemberScoreMaps(string fname) {
			unordered_map<string, unordered_map<string,long> > *m=&member_score_maps;
        	int count = 0;
        	if (m->empty()){
                return 0;
            }

        	FILE *fp = fopen(fname.c_str(), "w");
        	if (!fp){
                return -errno;
            }
			
			//putting entries into file as "key=score=member=score"
        	for(unordered_map<string, unordered_map<string,long> >::iterator it = m->begin(); it != m->end(); it++) {
        		string key=it->first;
        		for(unordered_map<string,long>::iterator mit=(it->second).begin();mit!=(it->second).end();mit++){
					stringstream ss;
        			string score,member;
        			ss<<(*mit).second;
        			score=ss.str();
        			member=(*mit).first;
                	fprintf(fp, "%s=%s=%s\n", key.c_str(), member.c_str(),score.c_str());
                	count++;	
				}
        	}

        	fclose(fp);
        	return count;
		}
		
		//reading from time_to_live file to the corresponding map
		int readTTL(char* fname){
			unordered_map<string, long> *m=&time_to_live;
			string s;
			ifstream inFile;
			inFile.open(fname);
			while(inFile>>s){
				string key="",ttlstr="";
				int l=s.length();
				int i=0;
				for(i=0;i<l;i++){
					if(s[i]=='=') break;
					key+=s[i];
				}
				i++;
				for(;i<l;i++){
					ttlstr+=s[i];
				}
				(*m)[key]=Redis::toInt(ttlstr);
			}
			inFile.close();
			return 1;
		}
		
		//reading from store file to the corresponding map
		int readStore(char* fname){
			unordered_map<string,pair<string,long> > *m=&store;
			string s;
			ifstream inFile;
			inFile.open(fname);
			while(inFile>>s){
				string key="",value="", time="";
				int l=s.length();
				int i=0;
				for(i=0;i<l;i++){
					if(s[i]=='=') break;
					key+=s[i];
				}
				i++;
				for(;i<l;i++){
					if(s[i]=='=') break;
					value+=s[i];
				}
				i++;
				for(;i<l;i++){
					time+=s[i];
				}
				long insert_time=Redis::toInt(time);
				(*m)[key]={value,insert_time};
			}
			inFile.close();
			return 1;
		}
		
		//reading from SortedSets file to the corresponding map
		int readSortedSets(char* fname){
			unordered_map<string,set<pair<long,string> > > *m=&sorted_sets;
			string s;
			ifstream inFile;
			inFile.open(fname);
			while(inFile>>s){
				string key="",member="", scorestr="";
				int l=s.length();
				int i=0;
				for(;i<l;i++){
					if(s[i]=='=') break;
					key+=s[i];
				}
				i++;
				for(;i<l;i++){
					if(s[i]=='=') break;
					scorestr+=s[i];
				}
				i++;
				for(;i<l;i++){
					member+=s[i];
				}
				long score=Redis::toInt(scorestr);
				(*m)[key].insert({score,member});
			}
			inFile.close();
			return 1;
		}
		
		//reading from MemberScoreMaps file to the corresponding map
		int readMemberScoreMaps(char* fname){
			unordered_map<string,unordered_map<string,long> > *m=&member_score_maps;
			string s;
			ifstream inFile;
			inFile.open(fname);
			while(inFile>>s){
				string key="",member="", scorestr="";
				int l=s.length();
				int i=0;
				for(i=0;i<l;i++){
					if(s[i]=='=') break;
					key+=s[i];
				}
				i++;
				for(;i<l;i++){
					if(s[i]=='=') break;
					member+=s[i];
				}
				i++;
				for(;i<l;i++){
					scorestr+=s[i];
				}
				long score=Redis::toInt(scorestr);
				(*m)[key][member]=score;
			}
			inFile.close();
			return 1;
		}
		
		
		//returns the time when maps were last refreshed
		int getLast_clean_time(){
			return last_clean_time;
		}
		
		//method to periodically refresh the ordered set and erase key-value pair which have expired
		void fixed_time_cleaning(){
			unordered_map<string, pair<string,long> >store_copy= store; //copy of store so that when we erase from original the iterator isn't affected
			unordered_map<string, pair<string,long> >::iterator store_iter=store_copy.begin(); //iterator of copied hashmap
			
			while(store_iter!=store_copy.end()){
				string key=store_iter->first;
				long insert_time=store_iter->second.second;
				long time_now=time(0);
				long time_elapsed=time_now-insert_time;
				long ttl=time_to_live[key];//time to live of that particular key
				
				if(time_to_live[key]!=-1 && time_elapsed>time_to_live[key]){
					//delete from the original hashmaps
					store.erase(store.find(key));
					time_to_live.erase(time_to_live.find(key));
				}
				store_iter++;
			}
			last_clean_time=time(0);//updating clean time
		}
		
		//function to bring ranges to positive number if they are given negative
		void setValidRanges(int* start_range,int* end_range,int size_set){
			if(*start_range<0) *start_range=size_set+*start_range;
			if(*end_range<0) *end_range=size_set+*end_range;
			*start_range=max(0,*start_range);
			*end_range=min(size_set-1,*end_range);
		}
		
		//gives the time to live of the asked key
		long TTL(string key){
			try{
				//if key is found in hashmap
				if(time_to_live.find(key)!=time_to_live.end()){
					return time_to_live[key];
				}
				//otherwise
				else{
					return -1;
				}
			}
			catch(...){
				cout<<"There occured some error while fetching the value";
				return -1;
			}
		}
		
		
		//sets the corresponding key-value pair and updates time to live to -1
		string SET(string key,string value){
			try{
				//if key is not found in hashmap
				if(store.find(key)==store.end()){
					//insert key-value into the hashmap
					store.insert({key,{value,-1}});
					time_to_live.insert({key,-1});
				}
				//if key is found in hashmap
				else{
					//update with new value
					store[key]={value,-1};
					//update it's TTL to -1 because old TTL no longer valid now
					time_to_live[key]=-1;
				}
			}
			catch(...){
				cout<<"There was a problem entering the key-value pair. Please try again";
				return nil;
			}
			
			return ok;
		}
		
		//return the value corresponding to the given key if it's present in the hashmap
		string GET(string key){
			try{
				//if key is present in hashmap
				if(store.find(key)!=store.end()){
					long time_now=time(0);
					long insert_time=store[key].second;
					long time_elapsed=time_now-insert_time;
					//checking if the key should have expired by now or not
					if(time_to_live[key]!=-1 && time_elapsed>time_to_live[key]){
						//if yes then erase that key from store as well as time_to_live map
						store.erase(store.find(key));
						time_to_live.erase(time_to_live.find(key));
						
						return nil;
					}
					//if not expired then return the value
					return store[key].first;
				}
				else{
					//if not found return NIL
					return nil; 
				}
			}
			catch(...){
				cout<<"There occured some error while fetching the value";
				return nil;
			}
		}
		
		
		//deletes the keys from store and retruns number of keys deleted
		int DEL(vector<string>keys){
			
			int num_deletions=0;//number of keys deleted which will be returned
			int size_keys=keys.size();
			
			for(int i=0;i<size_keys;i++){
				string key=keys[i];
				//if key is found in store
				if(store.find(key)!=store.end()){
					//delete from store
					store.erase(store.find(key));
					//delete from time_to_live also
					time_to_live.erase(time_to_live.find(key));
				}
				num_deletions++;
			}
			
			return num_deletions;
		}
		
		//add/update the time to live for the corresponding key
		int EXPIRE(string key,long expiry_time){
			//if key doesn't exist return 0
			if(time_to_live.find(key)==time_to_live.end()){
				return 0;
			}
			
			// start countdown from current time so enter current time in store hashmap
			string value=store[key].first;
			store[key]={value,time(0)};
			//update the value in time_to_live
			time_to_live[key]=expiry_time;
			return 1;
		}
		
		
		//add (member,score) pair to the ordered set of corresponding key
		int ZADD(string key,vector<pair<long,string> >score_member_list){
			
			int numOfElements=score_member_list.size();//number of pairs to be added in the set
			set<pair<long,string> >* cur_sorted_set;//to access ordered set of the corresponding key 
			unordered_map<string,long>* cur_member_score_map; //to access member-score map of corresponding key
			
			//if key is not present
			if(sorted_sets.find(key)==sorted_sets.end()){
				//create an ordered set for given key and also create member-score map
				sorted_sets.insert({key,set<pair<long,string> >()});
				member_score_maps.insert({key,unordered_map<string,long>()});
			}
			
			//assigning hashmaps by pointers so that we are accesing the true hashmaps and not the local ones
			cur_sorted_set=&sorted_sets[key];
			cur_member_score_map=&member_score_maps[key];
			
			
			pair<long,string>pair_elem;
			for(int i=0; i< numOfElements; i++){
				pair_elem=score_member_list[i];//each pair of the given list
				long score=pair_elem.first;
				string member=pair_elem.second;	
				
				//checking if the member is already not present
				if((*cur_member_score_map).find(member)==(*cur_member_score_map).end()){
					//it's not so insert the member and score into the oredered set
					(*cur_sorted_set).insert({score,member});
					(*cur_member_score_map).insert({member,score});
				}
				else{
					//if it was already present then
					//we need to delete this first and then update score of this member
					unordered_map<string,long>::iterator map_iter=(*cur_member_score_map).find(member);
					string del_member=map_iter->first; //member which is already present in the set
					long del_score=map_iter->second;    //previous score which should be updated
					
					//we will delete it now from our ordered set
					(*cur_sorted_set).erase((*cur_sorted_set).find({del_score,del_member}));
					
					//now we will insert the updated value in set and update the member-score map also
					(*cur_sorted_set).insert({score,member});
					(*cur_member_score_map)[member]=score;
				}
			}
			
			return numOfElements;//return number of elements entered in the ordered set
		}
		
		
		//returns a vector of members which lie in the given range
		vector<string> ZRANGE(string key,int start_range,int end_range){
			vector<string>members;//vector which we will return 
			set<pair<long,string> >* cur_sorted_set; //ordered set on which we will be working
			
			//if sorted set doesn't exist with given key
			if(sorted_sets.find(key)==sorted_sets.end()){
				return members;//empty vector
			}
			
			//assigning set by pointers so that we are accesing the true hashmaps and not the local ones
			cur_sorted_set=&sorted_sets[key];
			multiset<pair<long,string> >::iterator set_iter=(*cur_sorted_set).begin();//iterator of current ordered set
			int size_set=(*cur_sorted_set).size();//size of the set 
			setValidRanges(&start_range,&end_range,size_set);//setting valid values if ranges are negative

			int index=0;
			//moving set_iter to start_range location
			while(index<start_range){
				set_iter++;
				index++;
			}
			
			//iterating over each member of required range
			while(index<=end_range){
				string member=set_iter->second;
				//putting the member in answer vector
				members.push_back(member);
				index++;
				set_iter++;
			}
			
			return members;
		}
		
		//polymorphism with ZRANGE with change in number of parameters
		//returns a vector of members-score pair which lie in the given range
		vector<pair<string,long> > ZRANGE(string key,int start_range,int end_range,string withscore){
			vector<pair<string,long> >members;//vector of desired tuple which we will return
			set<pair<long,string> >* cur_sorted_set; //ordered set on which we will be working
			
			try{
			//if extra added parameter withscore is not valid
				if(withscore!="WITHSCORES"){
					throw withscore;
				}
			}
			
			catch(string exception){
				cout<<exception<<" is not a valid parameter\n";
				return members;
			}
			//if sorted set doesn't exist with given key
			if(sorted_sets.find(key)==sorted_sets.end()){
				return members;//empty vector
			}
			
			//assigning set by pointers so that we are accesing the true hashmaps and not the local ones
			cur_sorted_set=&sorted_sets[key];
			set<pair<long,string> >::iterator set_iter=(*cur_sorted_set).begin();//iterator of current ordered set
			int size_set=(*cur_sorted_set).size();//size of the set 
			setValidRanges(&start_range,&end_range,size_set);//setting valid values if ranges are negative

			int index=0;
			//moving set_iter to start_range location
			while(index<start_range){
				set_iter++;
				index++;
			}
			
			//iterating over each member of required range
			while(index<=end_range){
				string member=set_iter->second;
				long score=set_iter->first;
				//putting the member in answer vector
				members.push_back({member,score});
				index++;
				set_iter++;
			}
			
			return members;
		}
		
		
		//returns the rank of a member in a given ordered set
		int ZRANK(string key,string member){
			int rank=0;
			
			//current set and map of interest on which we will be working
			set<pair<long,string> >* cur_sorted_set;
			unordered_map<string,long>* cur_member_score_map;
			
			//if the key is not found return -1
			if(sorted_sets.find(key)==sorted_sets.end()){
				return -1;
			}
			
			//otherwise
			cur_sorted_set=&sorted_sets[key];//assigning by pointers
			cur_member_score_map=&member_score_maps[key];
			set<pair<long,string> >::iterator set_iter=(*cur_sorted_set).begin();//iterator on the current ordered set
			
			//find score of the member from member-score hashmap so that we can find the pair in the ordered set
			long score=(*cur_member_score_map)[member];
			pair<long,string>item_to_search=make_pair(score,member);
			
			//we start finding the pair now and keep the number of elements accessed till now to get the rank.
			while(set_iter!=(*cur_sorted_set).end()){
				if((*set_iter)==item_to_search){
					//if element is found no need to further iterate
					break;
				}
				rank++;
				set_iter++;
			}
			
			return rank;
		}
		
		
		//returns score as a string if key exists else returns NIL
		string ZSCORE(string key,string member){
			
			//if key doesn't exist in sorted set
			if(member_score_maps.find(key)==member_score_maps.end()){
				return nil;
			}
			
			unordered_map<string,long>* cur_member_score_map=&member_score_maps[key];
			//convert score to string
			string score= to_string((*cur_member_score_map)[member]);
			return score;
		}
};




int main(){
	Redis obj;
	
	string input;
	int flag=0;
	string inp;
	//taking the input from User
	while(getline(cin,inp)){
		stringstream ss(inp);
		//when program loads this creates the necessary files
		if(flag==0){
			string store="Store",ttl="time_to_live",sortedsets="SortedSets",memberscoremaps="MemberScoreMaps";
			FILE *fp = fopen(store.c_str(), "r");
        	if (!fp){
        		Redis::createFile(store);
			}
			fclose(fp);
			fp=fopen(ttl.c_str(),"r");
			if(!fp){
				Redis::createFile(ttl);	
			}
			fclose(fp);
			fp=fopen(sortedsets.c_str(),"r");
			if(!fp){
				Redis::createFile(sortedsets);	
			}
			fclose(fp);
			fp=fopen(memberscoremaps.c_str(),"r");
			if(!fp){
				Redis::createFile(memberscoremaps);	
			}
			fclose(fp);
			obj.initialize();
			flag=1;
		}
		ss>>input;
		if(input=="END"){
			obj.dumpData();
			break;
		}
		else if(input=="SET"){
			string key,value;
			ss>>key>>value;
			cout<<obj.SET(key,value)<<endl;
		}
		else if(input=="GET"){
			string key;
			ss>>key;
			cout<<obj.GET(key)<<endl;
		}
		else if(input=="ZADD"){
			string key;
			ss>>key;
			string score;string member;
			vector<pair<long,string> >score_member_list;
			while(ss>>score>>member){
				int scores=Redis::toInt(score);
				score_member_list.push_back({scores,member});
			}
			cout<<obj.ZADD(key,score_member_list)<<endl;
		}
		else if(input=="ZRANGE"){
			string key;
			ss>>key;
			string start_range,end_range;
			ss>>start_range>>end_range;
			int start_ranges=Redis::toInt(start_range);
			int end_ranges=Redis::toInt(end_range);
			string withscores="";
			ss>>withscores;
			if(withscores!=""){
				vector<pair<string,long> >members=obj.ZRANGE(key,start_ranges,end_ranges,withscores);
				for(int i=0;i<members.size();i++){
					cout<<members[i].first<<endl<<members[i].second<<endl;
				}
			}
			else{
				vector<string>members=obj.ZRANGE(key,start_ranges,end_ranges);
				for(int i=0;i<members.size();i++){
					cout<<members[i]<<endl;
				}
			}
		}
		else if(input=="ZRANK"){
			string key,member;
			ss>>key>>member;
			cout<<obj.ZRANK(key,member)<<endl;
		}
		else if(input=="EXPIRE"){
			string key;string ttl;
			ss>>key>>ttl;
			int ttls=Redis::toInt(ttl);
			cout<<obj.EXPIRE(key,ttls)<<endl;
		}
		else if(input=="TTL"){
			string key;
			ss>>key;
			cout<<obj.TTL(key)<<endl;
		}
		else if(input=="DEL"){
			vector<string>keys;
			string key;
			while(ss>>key){
				keys.push_back(key);
			}
			cout<<obj.DEL(keys)<<endl;
		}
		else if(input=="ZSCORE"){
			string key, member;
			ss>>key>>member;
			cout<<obj.ZSCORE(key,member)<<endl;
		}
	}
}
