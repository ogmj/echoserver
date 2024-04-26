#include "stdafx.h"

#include "TestReporter.h"

using namespace std;
namespace UnitTest
{

class TestReporterVisualC : public TestReporter
{
private:
	virtual void ReportTestStart(TestDetails const& /*test*/) {}
	virtual void ReportFailure(TestDetails const& details, char const* failure)
	{        
		char buffer[1024] = {0,};
		char const* const errorFormat = "%s(%d): error: Failure in %s: %s\n";
		sprintf_s(buffer, errorFormat, details.filename, details.lineNumber, details.testName, failure);
		OutputDebugString(buffer);
		cout << buffer;    
	}    
	virtual void ReportTestFinish(TestDetails const& /*test*/, float) {}
	virtual void ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed)
	{        
		TCHAR buffer[1024] = {0,};
		if (failureCount > 0)
		{            
			sprintf_s(buffer, "FAILURE: %d out of %d tests failed (%d failures).\n", failedTestCount, totalTestCount, failureCount);
		}
		else
		{
			sprintf_s(buffer, "Success: %d tests passed.\n", totalTestCount);
		}
		OutputDebugString(buffer);
		cout << buffer;
		sprintf_s(buffer, "Test time: %.2f seconds.\n", secondsElapsed);
		OutputDebugString(buffer);
		cout << buffer;
	}
};

}