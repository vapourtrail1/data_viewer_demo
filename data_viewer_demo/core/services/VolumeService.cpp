#include <vtkSmartPointer.h>
#include <vtkDICOMImageReader.h>
#include <vtkImageViewer2.h>
#include <vtkRenderWindowInteractor.h>

int main() {
	auto r = vtkSmartPointer<vtkDICOMImageReader>::New();
	r->SetDirectoryName("F:/data/xor_test"); // 你的 DICOM 序列文件夹
	r->Update();

	auto v = vtkSmartPointer<vtkImageViewer2>::New();
	v->SetInputConnection(r->GetOutputPort());
	v->Render();

	auto ia = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	v->SetupInteractor(ia);
	v->GetRenderer()->ResetCamera();
	ia->Start();
	return 0;
}

