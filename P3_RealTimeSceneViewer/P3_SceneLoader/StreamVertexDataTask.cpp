#include "StreamVertexDataTask.h"

StreamVertexDataTask::StreamVertexDataTask(int startingIndex, int max, std::string modelName, std::vector<float> data, grpc::ServerWriter<ModelData>* writer) 
	: startingIndex(startingIndex), max(max), modelName(modelName), data(data), writer(writer)
{

}

StreamVertexDataTask::~StreamVertexDataTask()
{

}

void StreamVertexDataTask::ExecuteTask()
{
	for (int i = startingIndex; i < max; i += 8)
	{
		Vector3* pos = new Vector3();
		pos->set_x(data[i]);
		pos->set_y(data[i + 1]);
		pos->set_z(data[i + 2]);

		Vector3* normals = new Vector3();
		normals->set_x(data[i + 3]);
		normals->set_y(data[i + 4]);
		normals->set_z(data[i + 5]);

		VertexData* vData = new VertexData();
		vData->set_allocated_position(pos);
		vData->set_allocated_normals(normals);
		vData->set_u(data[i + 6]);
		vData->set_v(data[i + 7]);

		ModelData mData;
		mData.set_modelname(modelName);
		mData.set_vdataindex(i / 8);
		mData.set_allocated_vdata(vData);

		writer->Write(mData);
	}
}