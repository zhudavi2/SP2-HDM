#ifndef _GOLEM_SDK_API_H_
#define _GOLEM_SDK_API_H_

#ifdef GOLEM_SDK_EXPORTS
#define GOLEM_SDK_API __declspec(dllexport)
#else
#define GOLEM_SDK_API __declspec(dllimport)
#endif

#pragma warning ( disable : 4251 )
#pragma warning ( disable : 4275 )

#endif // #ifndef _GOLEM_SDK_API_H_