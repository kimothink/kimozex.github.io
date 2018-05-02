

enum TIMER_COMMAND { TIMER_RESET, TIMER_START, TIMER_STOP, TIMER_ADVANCE,
                     TIMER_GETABSOLUTETIME, TIMER_GETAPPTIME, TIMER_GETELAPSEDTIME };

FLOAT __stdcall DXUtil_Timer( TIMER_COMMAND command );


class FrameSkip 
{
public: 
    inline void Clear() 
    { 
        m_Timer = 0.0f; 
    }
    inline void SetFramePerSec( float fps ) 
    { 
        m_SecPerFrame = 1.0f/fps;
    }
    
    // ���ϴ� �����Ӻ��� �ʹ� ������, 
    // false�� ��ȯ�ؼ� �ڵ带 ���۽�Ű�� �ʵ��� �Ѵ�.
    // dt�� '��'���� (�и��� �ƴ�!!!) 
    inline bool Update( float dt ) 
    {
        m_Timer += dt;
        if( m_Timer < 0 ) return false;
        
		// �������ӿ� �ش��ϴ� �ð��� ����. 
        m_Timer -= m_SecPerFrame;
        return true;
    }
    
    // Update�Ŀ� ȣ���ؼ� frame skip�� �����ؾ� �ϴ��� �˻��Ѵ�. 
    // frame skip�� �Ͼ�� �Ѵٸ� true�� ��ȯ�Ѵ�. 
    inline bool IsFrameSkip() 
    { 
        return m_Timer >= 0;
    }
    
    // ��������� ����/�Ҹ���. 
    FrameSkip(){ Clear(); } 
    virtual ~FrameSkip(){} 
protected: 
    float m_Timer; 
    float m_SecPerFrame; 
}; 
