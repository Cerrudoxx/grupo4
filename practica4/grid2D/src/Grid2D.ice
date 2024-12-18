//******************************************************************
// 
//  Generated by RoboCompDSL
//  
//  File name: Grid2D.ice
//  Source: Grid2D.idsl
//
//******************************************************************
#ifndef ROBOCOMPGRID2D_ICE
#define ROBOCOMPGRID2D_ICE
module RoboCompGrid2D
{
	struct TPoint
	{
		float x;
		float y;
		float radius;
	};
	sequence <TPoint> TPath;
	struct Result
	{
		TPath path;
		long timestamp;
		string errorMsg;
		bool valid;
	};
	interface Grid2D
	{
		Result getPaths (TPoint source, TPoint target);
	};
};

#endif
