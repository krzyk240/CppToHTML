#include <iostream>
#include <fstream>
#include <vector>
#include <queue>

using namespace std;

namespace aho
{
	namespace tree
	{
		struct node
		{
			int E[256], fail, long_sh_pat, pattern_id; // fail pointer, max shorter patter, pattern id
			bool is_pattern; // is pattern end in this vertex
			char color; // highlight color
			unsigned char character; // this node character
			node(unsigned char letter=0): is_pattern(false), character(letter)
			{
				for(int i=0; i<256; ++i)
					E[i]=0;
			}
			~node(){}
		};

		vector<node> graph;

		void init()
		{
			graph.resize(1); // add root
			graph[0].fail=graph[0].long_sh_pat=0; // max shorter pattern isn't exist
		}

		void add_word(const string& word, int id, char color)
		{
			int ver=0; // actual node (vertex)
			for(int s=word.size(), i=0; i<s; ++i)
			{
				if(graph[ver].E[word[i]]!=0) ver=graph[ver].E[word[i]]; // actual view node = next node
				else
				{
					ver=graph[ver].E[word[i]]=graph.size(); // add id of new node
					graph.push_back(node(word[i])); // add new node
				}
			}
			graph[ver].is_pattern=true;
			graph[ver].pattern_id=id;
			graph[ver].color=color;
		}

		void add_fails() // and the longest shorter patterns, based on BFS algorithm
		{
			queue<int> V;
			// add root childrens
			for(int i=0; i<256; ++i)
			{
				if(graph[0].E[i]!=0) // if children exists
				{
					graph[graph[0].E[i]].fail=graph[graph[0].E[i]].long_sh_pat=0;
					V.push(graph[0].E[i]);
				}
			}
			while(!V.empty())
			{
				int actual=V.front(); // id of actual view node
				for(int i=0; i<256; ++i) // i is character of view node
				{
					if(graph[actual].E[i]!=0) // if children exists
					{
						actual=graph[actual].fail; // we have view node parent's fial edge
						while(actual>0 && graph[actual].E[i]==0) // while we don't have node with children of actual character (i)
							actual=graph[actual].fail;
						actual=graph[graph[V.front()].E[i]].fail=graph[actual].E[i]; // the longest sufix, if 0 then longest sufix = root
						// add the longest shorter pattern
						if(graph[actual].is_pattern) // if the fail node is pattern then is long_sh_pat
							graph[graph[V.front()].E[i]].long_sh_pat=actual;
						else // long_sh_pat is the fail node's long_sh_pat
							graph[graph[V.front()].E[i]].long_sh_pat=graph[actual].long_sh_pat;
						actual=V.front();
						V.push(graph[actual].E[i]); // add this children to queue
					}
				}
				V.pop(); // remove visited node
			}
		}
	}

	vector<int> fin;

	void find(const string& text)
	{
		vector<tree::node>().swap(tree::graph); // clear tree::graph
		vector<int>(text.size()).swap(fin); // clear fin
		tree::init(); // initialize tree
		tree::add_fails(); // add fails edges
		int act=0, pat; // actual node - root
		for(int s=text.size(), i=0; i<s; ++i)
		{
			while(act>0 && tree::graph[act].E[text[i]]==0)
				act=tree::graph[act].fail; // while we can't add text[i] to path, go to fail node
			if(tree::graph[act].E[text[i]]!=0) // if we can add text[i] to path
				act=tree::graph[act].E[text[i]];
			if(tree::graph[act].is_pattern) // if actual node is pattern, then add it to fin
			{
				fin[i]=tree::graph[act].pattern_id;
			}
			else
			{
				pat=tree::graph[act].long_sh_pat; // go to the pattern node
				while(pat>0) // finding the longest pattern
				{
					if(tree::graph[pat].is_pattern)
					{
						fin[i]=tree::graph[pat].pattern_id; // add pat node to fin
						break;
					}
					pat=tree::graph[pat].long_sh_pat; // go to the next pattern
				}
			}
		}
	}
}

string synax_highlight(const string& code)
{
return code;
}

string code_coloring(const string& code) // coloring comments, preprocesor, chars and strings
{
	string ret, rest;
	for(int cl=code.size(), i=0; i<cl; ++i)
	{
		if(code[i]=='#') // preprocessor
		{
			ret+=synax_highlight(rest);
			rest="";
			ret+="<span class=\"p1\">";
			while(i<cl && code[i]!='\n')
			{
				if(i+1<cl && code[i]=='\\' && code[i+1]=='\n')
				{
					ret+="\\\n";
					++i;
				}
				else
				{
					switch(code[i])
					{
						case '<': ret+="&lt;";break;
						case '>': ret+="&gt;";break;
						case '&': ret+="&amp;";break;
						default: ret+=code[i];break;
					}
				}
				++i;
			}
			ret+="</span>";
			--i;
		}
		else if(i+1<cl && code[i]=='/' && code[i+1]=='/') // oneline comment
		{
			ret+=synax_highlight(rest);
			rest="";
			ret+="<span class=\"p8\">";
			while(i<cl && code[i]!='\n')
			{
				ret+=code[i];
				++i;
			}
			ret+="</span>";
			--i;
		}
		else if(i+1<cl && code[i]=='/' && code[i+1]=='*') // multiline comment
		{
			ret+=synax_highlight(rest);
			rest="";
			ret+="<span class=\"p8\">/*";
			i+=2;
			while(i<cl && !(code[i-1]=='*' && code[i]=='/'))
			{
				ret+=code[i];
				++i;
			}
			if(i<cl) // if comment ending
				ret+='/';
			ret+="</span>";
		}
		else if(code[i]=='"' || code[i]=='\'') // strings and chars
		{
			ret+=synax_highlight(rest);
			rest="";
			char str_or_char=code[i];
			ret+="<span class=\"p7\">";
			ret+=str_or_char;
			++i;
			while(i<cl && code[i]!=str_or_char)
			{
				if(code[i]=='\\')
				{
					++i;
					ret+="<span class=\"p6\">\\";
					if(i<cl) ret+=code[i];
					ret+="</span>";
				}
				else ret+=code[i];
				++i;
			}
			ret+=str_or_char;
			ret+="</span>";
		}
		else if(code[i]>='0' && code[i]<='9' && (i==0 || !code[i]=='_' || !(code[i]>='A' && code[i]<='Z') || !(code[i]>='a' && code[i]<='z'))) // numbers
		{
			ret+=synax_highlight(rest);
			rest="";
			 bool point=false;
			 ret+="<span class=\"p6\">";
			 ret+=code[i];
			 ++i;
			 while(i<cl && ((code[i]>='0' && code[i]<='9') || code[i]=='.'))
			 {
			 	if(code[i]=='.')
			 	{
			 		if(point) break;
			 		point=true;
			 	}
			 	ret+=code[i];
			 	++i;
			 }
			 if(i<cl && code[i]=='L')
			 {
			 	ret+='L';
			 	++i;
			 	if(i<cl && code[i]=='L')
			 	{
			 		ret+='L';
			 		++i;
			 	}
			 }
			 ret+="</span>";
			 --i;
		}
		else rest+=code[i];
	}
	ret+=synax_highlight(rest);
return ret;
}

int main(int argc, char **argv)
{
	ios_base::sync_with_stdio(false);
	if(argc<2)
	{
		cout << "Usage: CppToHTML <file>" << endl;
		return 0;
	}
	string file_name=argv[1];
	fstream file(file_name.c_str(), ios_base::in), output;
	if(!file.good()) cout << "Connot open file!" << endl;
	else
	{
		output.open((file_name+".html").c_str(), ios::out);
		string input, lol;
		int i=2;
		output << "<html>\n<head>\n<meta charset=\"utf-8\">\n<style>\ndiv:after\n{\n	content: \".\";\n	display: inline-block;\n	clear: both;\n	visibility: hidden;\n	line-height: 0;\n	height: 0;\n}\n\ndiv\n{\n	display: inline-block;\n}\n\nbody\n{\n  background: #272822;\n  color: #f8f8f2;\n  font-family: Helvetica, Arial, sans-serif;\n  font-size: 14px;\n}\n.code\n{\n  width: 40em;\n  border: 2px solid #49483e;\n  border-radius: 4px;\n}\n\n.cpp_code\n{\n  text-align: left;\n	margin: 0;\n  margin-left: 0px;\n  overflow: auto;\n  padding-left: 1em;\n  padding-bottom: 5px;\n  padding-top: 5px;\n  padding-right: 5px;\n  border-left: 2px solid #49483e;\n}\n\n.num_lines\n{\n  color: #8f908a;\n  float: left;\n  margin: 0px;\n  text-align: right;\n  padding-left: 3px;\n  padding-right: 5px;\n  padding-bottom: 5px;\n  padding-top: 5px;\n}\n\n.p1{color: #a6e22e;}\n.p2{color: #ff9b4b;}\n.p3{color: #f92672;}\n.p4{color: #66d9ef;}\n.p5{color: #b8b8b8;}\n.p6{color: #ae81ff;}\n.p7{color: #e6db74;}\n.p8{color: gray;}\n</style>\n</head>\n<body>\n<div class=\"code\">\n<pre class=\"num_lines\">\n";
		if(!file.eof())
		{
			getline(file, input);
			output << '1' << endl;
		}
		while(!file.eof())
		{
			input+='\n';
			getline(file, lol);
			input+=lol;
			output << i << "\n";
			++i;
		}
		file.close();
		output << "</pre>\n<pre class=\"cpp_code\">\n";
		//cout << input << endl;
		output << code_coloring(input) << endl;
		output << "</pre>\n</div>\n</body>\n</html>";
		output.close();
	}
return 0;
}