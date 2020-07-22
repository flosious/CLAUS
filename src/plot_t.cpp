#include "export.hpp"
#include "gnuplot_i.hpp" //Gnuplot class handles POSIX-Pipe-communikation with Gnuplot

/////////////////////////////////////////
////////////   PLOT   ///////////////////
/////////////////////////////////////////

void plot_t::fast_plot(quantity_t X, quantity_t Y, string outputfile) 
{
	bool logscale = false;
	
	
	Gnuplot g1;
// 	string outputfile = "/tmp/exports/"+ Y.name+ "_vs_" +X.name;
	g1.savetops(outputfile.c_str());
// 	g1.set_title(Y.name+ " vs " +X.name);
	g1.set_style("points");
	g1.set_pointsize(1);
	if (logscale) g1.set_ylogscale();
	g1.set_ylabel(Y.name + " [" + Y.unit + "]");
	g1.set_xlabel(X.name + " [" + X.unit + "]");
	g1.plot_xy(X.data ,Y.data, Y.name);
}


void plot_t::export_to_files(measurement_group_t* MG)
{	
	for (auto& measurement:MG->measurements)
	{
		plot_t plot(measurement);
	}
	return;
}

void plot_t::export_to_files(list<measurement_t>* measurements)
{	
	for (auto& measurement:*measurements)
	{
		plot_t plot(&measurement);
	}
	return;
}

void plot_t::export_to_files(list<measurement_group_t>& MGs)
{
	for (auto& MG:MGs)
		export_to_files(&MG);
}

plot_t::plot_t(measurement_t* measurement) : export2_t (measurement)
{
	if (conf.plots_location=="") return;
	bool logscale = true;
	bool save=true;
	
	string title, outputfile;
    tools::file::mkpath(root_directory(conf.plots_location),0750);
    outputfile = root_directory(conf.plots_location)+measurement->filename_p->filename()+"_PLOT";
	cout << "outputfile=" << outputfile<< endl;
	tools::str::replace_chars(&outputfile,"_",""); // \\\\U+005F
	outputfile = "/tmp/plots/test";
	cout << "outputfile=" << outputfile.c_str()<< endl;
	
    title=measurement->filename_p->filename();
    tools::str::replace_chars(&title,"_","\\\\_");
    Gnuplot g1;
    g1.set_utf8();
    g1.set_title(title);
    if (logscale) g1.set_ylogscale();
	vector<double> Y,X ;
// 	quantity_t Xq;
// 	if (save) g1.savetops(outputfile.c_str());
	for (auto& cluster:measurement->clusters)
	{
		
//         bool out=false;
//         for (int i=0;i<element_names.size();i++) {
//             if (it->first == element_names[i]) { out = true; break;}
//         }
//         if (out || element_names.size()==0) {

            /*create the actual plot or its temporary files*/
            
			string plotname;
			
            /*raw_data*/
            if (save) g1.savetops(outputfile.c_str());
            g1.set_style("points");
            g1.set_pointsize(0.05);
			
			
			if (cluster.second.concentration().is_set())
			{
				Y = cluster.second.concentration().data;
				g1.set_ylabel(cluster.second.concentration().name + "["+ cluster.second.concentration().unit + "]");
				plotname=cluster.second.name()+"\\\\_concentration";
			}
			else
			{
				Y = cluster.second.intensity().data;
				g1.set_ylabel(cluster.second.intensity().name + "["+ cluster.second.intensity().unit + "]");
				plotname=cluster.second.name()+"\\\\_intensity";
			}
			if (cluster.second.sputter_depth().is_set() )
			{
				X = cluster.second.sputter_depth().data;
				g1.set_xlabel(cluster.second.sputter_depth().name + "["+ cluster.second.sputter_depth().unit + "]");
			}
			else
			{
				X = cluster.second.sputter_time().data;
				g1.set_xlabel(cluster.second.sputter_time().name + "["+ cluster.second.sputter_time().unit + "]");
			}
// 			cout << "outputfile: " << outputfile << endl;
// 			cout << "title: " << title << endl;
// 			cout << "plotname: " << plotname << endl;
// 			cout << "cluster.name()=" << cluster.second.name() << endl;
// 			cout << "Y.size()=" << Y.size() << endl;
// 			cout << "X.size()=" << X.size() << endl;
/////	/// SKIP
// 			if (statistics::get_median_from_Y(Y)>1E6) continue;
			
			if (X.size()==Y.size() && 0!=Y.size())
				g1.plot_xy(X ,Y,plotname);
// 				g1.plot_xy(X ,Y);
			else
				cout << "X.size()!=y.size(): " << outputfile << endl;
            

// 			 /*equilibrium*/
//             if (save) g1.savetops(outputfile.c_str());
//             g1.set_style("lines");
//             if (cluster.second.equilibrium().concentration().is_set()) Y = cluster.second.equilibrium().concentration().data;
// 			else Y = cluster.second.equilibrium().intensity().data;
// 			if (cluster.second.equilibrium().sputter_depth().is_set() ) X = cluster.second.equilibrium().sputter_depth().data;
// 			else X = cluster.second.equilibrium().sputter_time().data;
// 			
// 			if (X.size()==Y.size()  && 0!=Y.size())
// 				g1.plot_xy(X ,Y,cluster.second.name()+"\\\\_equilibrium");
// 			else
// 				cout << "X.size()!=y.size(): equilibrium: " << outputfile << endl;
            
        
    }
        
    vector<string> files=g1.get_tmpfiles();
    files_to_delete.insert(files_to_delete.end(),files.begin(),files.end());
    return;
}

plot_t::~plot_t()
{
	tools::file::remove_files(files_to_delete);
	return;
}

void plot_t::XY(map<double,double> XY, string outputfile) 
{
	vector<double> X,Y;
	tools::vec::split_map_to_vecs(&XY,&X,&Y);
	plot_t::XY(X,Y,outputfile);
	return;
}

void plot_t::XY(vector<double> X, vector<double> Y,string outputfile) 
{
    Gnuplot g;
    g.set_title("TEST");
    g.set_ylogscale();
    g.savetops("/tmp/"+outputfile);
    g.set_style("lines");
    g.plot_xy(X ,Y);
    return;
}
