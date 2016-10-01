
#include "../Internal.h"
#include <windows.h>

LN_NAMESPACE_BEGIN
namespace detail {

class GenericFileFinderImpl
	: public GenericFileFinderImplBase<char>
{
public:
	GenericFileFinderImpl(const GenericStringRef<char>& dirPath)
		: GenericFileFinderImplBase<char>(dirPath)
		, m_dir(NULL)
	{
		StringA t;
		t.AssignCStr(GenericFileFinderBase<TChar>::m_dirPath.c_str());
		Initializ(t.c_str());
	}

	~GenericFileFinderImpl()
	{
		if (m_dir != NULL)
		{
			closedir(m_dir);
		}
	}

	void Initialize(const char* dirPath)
	{
		m_dir = opendir(dirPath);
		LN_THROW(m_dir != NULL, IOException, dirPath);

		Next();
	}

	virtual bool Next() override
	{
		struct dirent* entry;
		do
		{
			entry = readdir(m_dir);
			if (entry)
			{
				GenericFileFinderBase<TChar>::SetCurrentFileName(entry->d_name);
			}
			else
			{
				GenericFileFinderBase<TChar>::SetCurrentFileName((char*)NULL);
				break;
			}
		} while (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0);

		return !GenericFileFinderBase<TChar>::GetCurrent().IsEmpty();
	}

private:
	DIR*	m_dir;
};

} // namespace detail
LN_NAMESPACE_END
