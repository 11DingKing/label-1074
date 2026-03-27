#include <QTest>
#include "test_gameobject.h"
#include "test_player.h"
#include "test_enemy.h"
#include "test_collision.h"
#include "test_logger.h"

/**
 * 测试主入口
 * 运行所有测试套件
 */
int main(int argc, char *argv[])
{
    int status = 0;
    
    // 运行各测试类
    {
        TestGameObject test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    {
        TestPlayer test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    {
        TestEnemy test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    {
        TestCollision test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    {
        TestLogger test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    return status;
}
