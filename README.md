# Redis
An implementation of basic Redis functionalities using C++ 

Redis is a basic key value store. The key stores the value and that value can be retrieved if we know the key. Another important feature is that it is A a Persistent storage i.e. the data is always stored somewhere whether the application is running or not so Data is never lost.

# Language Used

I have used C++ to implement Redis. Since implementing Redis required a lot of functions to deal with implying that one should use an Object Oriented Programming(OOP) language therefore I used C++ which supports OOPs behaviour. I could have used Java also since I know Java but I had been using C++ quite frequently for Competitive Coding so I was more comfortable(fast implementation) implementing it with C++ and also because there was no Use Case which would be a hindrance with Cpp.

# How did I store the data ?

My first instinct was of using a MySQL database but then after giving it a considerable amount of thought I concluded that it would be an overkill for these basic functionalities. Therefore I decided to go with a simple File Database which was much easier to set up and manage. Also there was not much need for complex querying which would require a Relational database. I maintained a file for each of the four Hashmaps and when the program first loads the data from the files is written to the maps. And when the user ends the application by entering END command then the data from the maps is put back into the corresponding files. In this way we never lose the data.

# Which Data Structures I have used and why?

1. For storing the key value pair I have used a hashmap with value as a pair-> {value,insert_time} where value is the value associated with the key and insert_time is the time at which this key was inserted/updated. The insert time is important because of the EXPIRY query. Whenever a Expiry is set for a key we insert time at that moment as the insertion time of the key and when the current time exceeds the insertion time by the Expiry set the key is removed from the hashmap. Therefore this insert time is necessary. Since we do not require ordering on this key-value store therefore I have used an unordered_map(Hashmap) and not a map(Balanced Binary Search Tree). This map handles SET and GET queries.
2. I have used another hashmap(unordered_map) for storing the expiry time of the keys. This map handles EXPIRY query.
3. For the sorted sets I have used a Hashmap(unordered_map) whose key is the string key that the user enters and whose value is an ordered set(set) containing a pair-> {score,member}. I’ve used a set because here we want ordering on the data for a particular key based on the score and a C++ set stores data in ascending order which serves our purpose. The first value in the pair is score which sorts the data based on the score and the second value is the member which sorts the members lexicographically if two scores are equal(ZRANGE/ZRANK query). Now for the pair used it is used because there will always be a unique {score,member} pair because a member can’t have two scores therefore this pair is always unique. Since we don’t not require ordering by keys for the sorted sets the outer map is an unordered_map.
4. I have used another hashmap for storing the score of a particular member so that ZSCORE query can be resolved efficiently. In this hashmap the key is the key of the sorted set and the value is another hashmap which maps the member to it’s score. Since we neither require ordering by the key nor ordering by member for this map therefore an unordered_map is used as the outer as well as the inner map.
5. The above two hashmaps handle the ZADD,ZRANGE,ZRANK,ZSCORE queries.

# Further Improvements that can be made

1. We are reading from the files when the application starts and dumping the data into the files when the application ends. Instead if we use MySql database then we can remove the data from the database as soon as it is cleared from the table. In the present solution the database contains the data (even if it is removed from maps during runtime) till the application ends which can be improved although the present solution doesn’t give errors because we don’t read from file once the program has started(only once file is read during initialization).
2. We have the code of initialization(which happens only once) in the Redis class itself but since it is done only one time when the App loads we can have a Singleton Class for it which will return the maps filled with data read from the files. This will make it more modular. (I didn’t implement it because of lack of time and also I had a fever).
3. We can also use a hashmap for rank which will give the rank of a member in a sorted set in O(1) time. We would have to update that hashmap whenever insertion occurs in that sorted set.
4. We can schedule the refreshing of maps(remove keys which have expired) periodically with some scheduler which will be more efficient.

# Does the Application support multithreading?

Currently the Application doesn’t support multi threaded operations. But yes as a further modification the App can be made to support multi threaded operations. Multi threaded operations will cause a problem when two threads try to access a map(shared resource) and try to write/update a value. This will lead to an inconsistent situation with one or more threads having stale data. So to remove that we will have to use a Mutex or a Semaphore which can be implemented pretty easily in any language and it will allow only one thread to access the shared resource at one time. 


# Summary
So as seen from the images above we have created a basic working Redis App which allows features like SET, GET, EXPIRE, ZADD, ZRANK, ZRANGE, TTL, ZSCORE, DEL.
And most importantly the data is always stored somewhere apart from the Data Structures so that it is not lost even when we close the Application i.e. It is Persistent.

# How to interact with the Application?
1. Everything you enter should be without quotes.  The code takes care of the Data types.
2. Every two adjacent inputs will be separated by a single space.
3. To use SET command enter SET key value. Key and value are strings.
4. To use GET command enter GET key. You’ll get the value of the key or nil string if key doesn’t exist.
5. To use EXPIRE command enter EXPIRE key time. Sets the expire time of the key.
6. To use the ZADD command enter ZADD key score1 member1 score2 member2 ….. Add as many members you want to the sorted set.
7. To use ZRANK command enter ZRANK key member. Returns the rank of the member.
8. To use ZRANGE command enter ZRANGE key start_index end_index OR ZRANGE key start_index end_index WITHSCORES
9. To use ZSCORE command enter ZSCORE key member
10. To use TTL command enter TTL key
11. To use DEL command enter key1 key2 key 3….Enter as many keys as you want to delete.
12. To end the Program enter END.

# Implemented by:
         aAbhirav Sati
				 IIT Patna
				 Contact No: 9955527784
				 Email: abhi04.sati@gmail.com

