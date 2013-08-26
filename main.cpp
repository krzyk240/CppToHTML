#include "main.hpp"

/*
p1 - green
p2 - orange
p3 - red
p4 - turquoise italic
p41 - turquoise
p5 - light gray
p6 - purple
p7 - yellow
p8 - gray
p9 - blue
*/

Color_schemes color_scheme=sublime;

string to_string(int a)
{
	stack<unsigned char> st;
	while(a>0)
	{
		st.push('0'+a%10);
		a/=10;
	}
	string w;
	while(!st.empty())
	{
		w+=st.top();
		st.pop();
	}
	if(w.empty()) w="0";
return w;
}

bool is_name[256]={}, is_true_name[256]={};

void compress(string file_name)
{
	fstream file;
	file.open(file_name.c_str(), ios_base::in);
	string k, g, m, h;
	stack<string> base;
	unsigned char hz=file.get();
	while(file.good())
	{
		k+=hz;
		hz=file.get();
	}
	file.close();
	file.open(file_name.c_str(), ios_base::out);
	for(unsigned int i=0; i<k.size(); ++i)
	{
		if(i+6<k.size() && k[i]=='<' && k[i+1]=='/' && k[i+2]=='s' && k[i+3]=='p' && k[i+4]=='a' && k[i+5]=='n' && k[i+6]=='>')
		{
			i+=6;
			string buff;
			while(k[++i]==' ' || k[i]=='\n' || k[i]=='\t')
				buff+=k[i];
			if(i+4<k.size() && k[i]=='<' && k[i+1]=='s' && k[i+2]=='p' && k[i+3]=='a' && k[i+4]=='n')
			{
				i+=5;
				m="<span";
				while(i<k.size() && k[i]!='>')
				{
					m+=k[i];
					++i;
				}
				m+='>';
				if(g!=m){file << "</span>" << buff << m;g=m;}
				else file << buff;
			}
			else
			{
				file << "</span>" << buff;
				g="";
				i--;
			}
		}
		else if(i+4<k.size() && k[i]=='<' && k[i+1]=='s' && k[i+2]=='p' && k[i+3]=='a' && k[i+4]=='n')
		{
			i+=5;
			m="<span";
			while(i<k.size() && k[i]!='>')
			{
				m+=k[i];
				++i;
			}
			m+='>';
			if(g!=m){file << m;g=m;}
		}
		else file << k[i];
	}
	file.close();
}

int main(int argc, char **argv)
{
	ios_base::sync_with_stdio(false);
	if(argc<2)
	{
		cout << "Usage: CppToHTML [options] <file>" << endl;
		return 0;
	}
	string file_name;
	for(int i=1; i<argc; ++i)
	{
		if(argv[i][0]=='-' && argv[i][1]=='c')
		{
			string col_scheme(argv[i]+2);
			if(col_scheme.empty() && ++i<argc) col_scheme.assign(argv[i]);
			if(col_scheme=="sublime") color_scheme=sublime;
			else if(col_scheme=="codeblocks") color_scheme=codeblocks;
			else cout << "Undefined color scheme!\nColor schemes:\n    sublime\n    codeblocks" << endl;
		}
		else if(string(argv[i])=="--help")
			cout << "Usage: CppToHTML [options] <file>\nOptions:\n    -c <color_scheme>   Color schemes: sublime, codeblocks" << endl;
		else file_name=argv[i];
	}
	if(file_name.empty())
	{
		cout << "You must specify the file name!\nUsage: CppToHTML [options] <file>" << endl;
		return 0;
	}
	// make is_name array
	is_true_name['_']=is_name['_']=true;
	for(int i='A'; i<='Z'; ++i)
		is_true_name[i]=is_name[i]=true;
	for(int i='a'; i<='z'; ++i)
		is_true_name[i]=is_name[i]=true;
	for(int i='0'; i<='9'; ++i)
		is_true_name[i]=true;
	// get search includes directories and actual path - only for linux!
	// includes
	fstream settings("settings.cfg", ios_base::in);
	if(settings.good())
	{
		string path;
		while(!settings.eof())
		{
			getline(settings, path);
			parser::include_directories.push_back(path);
		}
	}
	else cerr << "Cannot load settings.cfg file with include directories!\n";
	// path
	if(file_name[0]!='/')
	{
		if(system("pwd > lol.shell"));
		fstream lol("lol.shell", ios_base::in);
		string path;
		if(lol.good())
		{
			getline(lol, path);
			lol.close();
		}
		if(system("rm -f lol.shell"));
		if(*--path.end()!='/') path+='/';
		file_name=path+file_name;
	}
	parser::init();
	parser::parse_file(file_name);
	// ------------------
	fstream file(file_name.c_str(), ios_base::in), output;
	if(!file.good()) cout << "Connot open file!" << endl;
	else
	{
		output.open((file_name+".html").c_str(), ios_base::out);
		string input, lol;
		int i=2;
		switch(color_scheme)
		{
			case sublime: output << "<html>\n<head>\n<meta charset=\"utf-8\">\n<style>\ndiv:after\n{\n	content: \".\";\n	display: inline-block;\n	clear: both;\n	visibility: hidden;\n	line-height: 0;\n	height: 0;\n}\n\ndiv\n{\n	display: inline-block;\n}\n\nbody\n{\n  background: #272822;\n  color: #f8f8f2;\n  font-family: Helvetica, Arial, sans-serif;\n  font-size: 14px;\n}\n.code\n{\n  width: 100%;\n  border: 2px solid #49483e;\n  border-radius: 4px;\n}\n\n.cpp_code\n{\n  text-align: left;\n	margin: 0;\n  margin-left: 0px;\n  overflow: auto;\n  padding-left: 1em;\n  padding-bottom: 5px;\n  padding-top: 5px;\n  padding-right: 5px;\n  border-left: 2px solid #49483e;\n}\n\n.num_lines\n{\n  color: #8f908a;\n  float: left;\n  margin: 0px;\n  text-align: right;\n  padding-left: 3px;\n  padding-right: 5px;\n  padding-bottom: 5px;\n  padding-top: 5px;\n}\n\n.p1{color: #a6e22e;}\n.p2{color: #ff9b4b;}\n.p3{color: #f92672;}\n.p4{color: #66d9ef;font-style: italic;}\n.p41{color: #66d9ef;}\n.p42{color: #a6e22e;}\n.p5{color: #b15555;}\n.p6{color: #ae81ff;}\n.p7{color: #e6db74;}\n.p71{color: #e6db74;}\n.p8{color: #75715e;}\n.p9{color: #3c74ec;}\n</style>\n</head>\n<body>\n<div class=\"code\">\n<pre class=\"num_lines\">\n";break;
			case codeblocks: output << "<html>\n<head>\n<meta charset=\"utf-8\">\n<style>\ndiv:after\n{\n	content: \".\";\n	display: inline-block;\n	clear: both;\n	visibility: hidden;\n	line-height: 0;\n	height: 0;\n}\n\ndiv\n{\n	display: inline-block;\n}\n\nbody\n{\n  background: #ffffff;\n  color: #000000;\n  font-family: Helvetica, Arial, sans-serif;\n  font-size: 14px;\n}\n.code\n{\n  width: 100%;\n  border: 1px solid #afafaf;\n  border-radius: 4px;\n}\n\n.cpp_code\n{\n  text-align: left;\n	margin: 0;\n  margin-left: 0px;\n  overflow: auto;\n  padding-left: 1em;\n  padding-bottom: 5px;\n  padding-top: 5px;\n  padding-right: 5px;\n  border-left: 1px solid #afafaf;\n}\n\n.num_lines\n{\n  color: #4c4c4c;\n  float: left;\n  margin: 0px;\n  text-align: right;\n  padding-left: 3px;\n  padding-right: 5px;\n  padding-bottom: 5px;\n  padding-top: 5px;\n}\n\n.p1{color: #00a000;}\n.p2{color: #e20deb;font-weight: bold;}\n.p3{color: #0000ff;font-weight: bold;}\n.p4{color: #0000ff;font-weight: bold;}\n.p41{color: #1c8ce5;font-weight: bold;}\n.p42{color: #e20deb;font-weight: bold;}\n.p5{color: #ff0000;}\n.p6{color: #f000f0;}\n.p7{color: #0000ff;}\n.p71{color: #e0a000;}\n.p8{color: #a0a0a0;}\n.p9{color: #00a000;font-weight: bold;}\n</style>\n</head>\n<body>\n<div class=\"code\">\n<pre class=\"num_lines\">\n";break;
		}
		if(!file.eof())
		{
			getline(file, lol);
			// input+=lol (tab_size=4)
			for(int ll=lol.size(), q=0; q<ll; ++q)
			{
				if(lol[q]=='\t') input+="    ";
				else input+=lol[q];
			}
			output << '1' << endl;
		}
		while(!file.eof())
		{
			input+='\n';
			getline(file, lol);
			// input+=lol (tab_size=4)
			for(int ll=lol.size(), q=0; q<ll; ++q)
			{
				if(lol[q]=='\t') input+="    "; // tab size: 4
				else input+=lol[q];
			}
			output << i << '\n';
			++i;
		}
		file.close();
		output << "</pre>\n<pre class=\"cpp_code\">\n";
		//cout << input << endl;
		output << synax_highlight::code_coloring(input) << endl;
		output << "</pre>\n</div>\n</body>\n</html>";
		output.close();
		compress(file_name+".html");
	}
return 0;
}