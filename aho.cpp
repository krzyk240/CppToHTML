#include "main.hpp"

aho::aho_trie::aho_trie(): graph(1) // add root
{
	graph[0].fail=graph[0].long_sh_pat=0; // max shorter pattern isn't exist
}

int aho::aho_trie::add_word(const string& word, int id, unsigned char color)
{
	int ver=0; // actual node (vertex)
	for(int s=word.size(), i=0; i<s; ++i)
	{
		if(graph[ver].E[static_cast<int>(word[i])]!=0) ver=graph[ver].E[static_cast<int>(word[i])]; // actual view node = next node
		else
		{
			ver=graph[ver].E[static_cast<int>(word[i])]=graph.size(); // add id of new node
			graph.push_back(node(static_cast<int>(word[i]))); // add new node
		}
	}
	graph[ver].is_pattern=true;
	graph[ver].pattern_id=id;
	graph[ver].color=color;
	graph[ver].depth=word.size();
return ver;
}

void aho::aho_trie::add_fails() // and the longest shorter patterns, based on BFS algorithm
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

void aho::find(const string& text, int x)
{
	vector<int>(text.size()).swap(fin); // clear fin
	int act=0, pat; // actual node - root
	for(int s=text.size(), i=0; i<s; ++i)
	{
		while(!troll.empty() && troll.front().pos<=x+i-text.size())
		{
			trie.graph[troll.front().id].is_pattern=troll.front().is_pattern;
			troll.pop();
		}
		//--------------------------------------
		while(act>0 && trie.graph[act].E[static_cast<int>(text[i])]==0)
			act=trie.graph[act].fail; // while we can't add text[i] to path, go to fail node
		if(trie.graph[act].E[static_cast<int>(text[i])]!=0) // if we can add text[i] to path
			act=trie.graph[act].E[static_cast<int>(text[i])];
		if(trie.graph[act].is_pattern) // if actual node is pattern, then add it to fin
		{
			fin[i-trie.graph[act].depth+1]=act;
		}
		else
		{
			pat=trie.graph[act].long_sh_pat; // go to the pattern node
			while(pat>0) // finding the longest pattern
			{
				if(trie.graph[pat].is_pattern)
				{
					fin[i-trie.graph[pat].depth+1]=pat; // add pat node to fin
					break;
				}
				pat=trie.graph[pat].long_sh_pat; // go to the next pattern
			}
		}
	}
}