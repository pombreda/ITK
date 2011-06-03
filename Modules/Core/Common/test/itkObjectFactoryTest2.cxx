/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkImage.h"
#include "itkRGBPixel.h"
#include "itksys/SystemTools.hxx"
#include <string>
#include <list>

typedef itk::ImportImageContainer<unsigned long,short>::Pointer myPointer;
bool TestNew2(myPointer v, const char* expectedClassName)
{

  std::cout << "v->GetNameOfClass(): " << v->GetNameOfClass();
  std::cout << ", expectedClassName: " << expectedClassName << std::endl;
  if(strcmp(v->GetNameOfClass(), expectedClassName) != 0)
    {
    std::cout << "Test Failed" << std::endl;
    return false;
    }
  return true;
}


template<class T>
void MakeImage(const int count, T pixel)
{
  typedef itk::Image<T, 3>      ImageType;
  typedef typename ImageType::IndexType  IndexType;
  typedef typename ImageType::RegionType RegionType;
  typedef typename ImageType::SizeType   SizeType;

  typename ImageType::Pointer  testImage    = ImageType::New();

  IndexType index;
  index[0] = 0;
  index[1] = 0;
  index[2] = 0;

  SizeType size;
  size[0] = count;
  size[1] = count;
  size[2] = count;
  RegionType region;
  region.SetSize( size );
  region.SetIndex( index );

  testImage->SetRegions( region );
  testImage->Allocate();
  testImage->FillBuffer(pixel);
}

void ReallocateImage()
{
  typedef itk::Image<double, 2> ImageType;
  typedef ImageType::SizeType   SizeType;

  ImageType::Pointer testImage = ImageType::New();

  SizeType size = {{5, 3}};

  testImage->SetRegions( size );
  testImage->Allocate();
  testImage->FillBuffer( 0 );

  SizeType size2 = {{100, 100}};
  testImage->SetRegions( size2 );
  testImage->Allocate();
  testImage->FillBuffer( 0 );
}

int itkObjectFactoryTest2(int argc, char *argv[])
{
  itk::ObjectFactoryBase::UnRegisterAllFactories();
  if (argc < 2)
    {
    std::cout << "Usage: " << argv[0] << " FactoryPath" << std::endl;
    return EXIT_FAILURE;
    }

  std::string myenv = std::string("ITK_AUTOLOAD_PATH=") + std::string(argv[1]) + std::string("/");
#ifdef CMAKE_INTDIR
  myenv += std::string(CMAKE_INTDIR);
#endif
  std::cout << myenv << std::endl;
  putenv (const_cast<char *>(myenv.c_str()));
  itk::ObjectFactoryBase::ReHash();

  // List all registered factories
  std::list<itk::ObjectFactoryBase *> factories =
    itk::ObjectFactoryBase::GetRegisteredFactories();

  std::cout << "----- Registered factories -----" << std::endl;
  if (factories.size() > 0)
    {
    for ( std::list<itk::ObjectFactoryBase*>::iterator
            f = factories.begin();
          f != factories.end(); ++f )
      {
      std::cout << "  Factory version: "
                << (*f)->GetITKSourceVersion() << std::endl
                << "  Factory description: "
                << (*f)->GetDescription() << std::endl;

      std::list<std::string> overrides = (*f)->GetClassOverrideNames();
      std::list<std::string> names = (*f)->GetClassOverrideWithNames();
      std::list<std::string> descriptions = (*f)->GetClassOverrideDescriptions();
      std::list<bool> enableflags = (*f)->GetEnableFlags();
      std::list<std::string>::const_iterator n = names.begin();
      std::list<std::string>::const_iterator d = descriptions.begin();
      std::list<bool>::const_iterator e = enableflags.begin();
      for ( std::list<std::string>::const_iterator o = overrides.begin();
            o != overrides.end(); ++o, ++n, ++d, e++ )
        {
        std::cout << "    Override " << *o
                  << " with " << *n << std::endl
                  << "      described as \"" << *d << "\"" << std::endl
                  << "      enabled " << *e << std::endl;
        }
      }
    std::cout << "----- -----" << std::endl;
    }
  else
    {
    std::cout << "Failed to load any factories" << std::endl;
    return EXIT_FAILURE;
    }

  itk::ImportImageContainer<unsigned long,short>::Pointer v = itk::ImportImageContainer<unsigned long,short>::New();
  if (!TestNew2(v, "TestImportImageContainer"))
    {
    return EXIT_FAILURE;
    }

  MakeImage(10, static_cast<short>(0));
  MakeImage(10, static_cast<unsigned char>(0));
  MakeImage(10, static_cast<int>(0));
  {
  MakeImage(10, static_cast<float>(0));
  MakeImage(10, static_cast<double>(0));
  }
  itk::RGBPixel<unsigned char> rgbUC; rgbUC.Fill(0);
  itk::RGBPixel<unsigned short> rgbUS; rgbUS.Fill(0);
  MakeImage(10, rgbUC);
  MakeImage(10, rgbUS);

  ReallocateImage();

  int status = EXIT_SUCCESS;

  return status;
}

