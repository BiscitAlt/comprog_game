#include "skill.h"
#include "raymath.h"
#include <algorithm>

static std::vector<Vector2> lightningPoints;
//หา enemy ใกล้สุดในระยะ
Enemy* FindClosestEnemy(Vector2 pos,std::vector<Enemy>& enemies,float range)
{
    Enemy* target = nullptr;
    float closest = range;

    for(auto &e : enemies)
    {
        float dist = Vector2Distance(pos,e.pos);

        if(dist < closest)
        {
            closest = dist;
            target = &e;
        }
    }

    return target;
}
// UpdateSkills 
void UpdateSkills(
    SkillState &s,
    player &pl,
    std::vector<Enemy> &enemies,
    float dt
)
{
    // Timer เวลาคลูดาว
    s.voidMeteorTimer += dt;
    s.chainLightningTimer += dt;
    s.poisonMistTimer += dt;
    s.blackHoleTimer += dt;
    s.bloodAuraTimer += dt;
    s.shockwaveTimer += dt;

    // VOID METEOR
    if(s.voidMeteorTimer > 3.0f)
    {
        Enemy* target = FindClosestEnemy(pl.pos,enemies,350);

        if(target)
        {
            s.effectPos = target->pos;
            s.voidMeteorEffect = 0.8f;

            for(auto &e : enemies)
            {
                float dist = Vector2Distance(e.pos,target->pos);

                if(dist < 100)
                {
                    e.hp -= 40;
                }
            }
        }

        s.voidMeteorTimer = 0;
    }

    // PHANTOM BLADES
        if(s.phantomBladeTimer >= 0)
{
        s.phantomBladeAngle += dt * 2.5f;

        float radius = 70;

        for(int i=0;i<3;i++)
    {
        float angle = s.phantomBladeAngle + i * 2*PI/3;

        Vector2 bladePos =
        {
            pl.pos.x + cosf(angle)*radius,
            pl.pos.y + sinf(angle)*radius
        };

        Rectangle bladeRec =
        {
            bladePos.x-8,
            bladePos.y-8,
            16,
            16
        };

        for(auto &e:enemies)
        {
            Rectangle enemyRec =
            {
                e.pos.x,
                e.pos.y,
                e.size.x,
                e.size.y
            };

            if(CheckCollisionRecs(bladeRec,enemyRec))
            {
                e.hp -= 20 * dt;
            }
        }
    }
}

    // CHAIN LIGHTNING
    if(s.chainLightningTimer > 3.0f)
{
    lightningPoints.clear();

    Enemy* target = FindClosestEnemy(pl.pos,enemies,400);

    if(target)
    {
        float damage = 35;

        Vector2 currentPos = target->pos;

        std::vector<Enemy*> hitEnemies;

        for(int i=0;i<8;i++)
        {
            Enemy* next = FindClosestEnemy(currentPos,enemies,300);

            if(!next) break;

            // กันตีตัวเดิม
            if(std::find(hitEnemies.begin(),hitEnemies.end(),next) != hitEnemies.end())
                break;

            lightningPoints.push_back(currentPos);
            lightningPoints.push_back(next->pos);

            next->hp -= damage;

            hitEnemies.push_back(next);

            damage *= 0.85f;

            currentPos = next->pos;
        }

        s.chainLightningEffect = 0.4f;
    }

    s.chainLightningTimer = 0;
}

    // POISON MIST
    if(s.poisonMistTimer > 6.0f)
    {
        Enemy* target = FindClosestEnemy(pl.pos,enemies,350);

        if(target)
        {
            s.effectPos = target->pos;
            s.poisonMistEffect = 2.0f;
        }

        s.poisonMistTimer = 0;
    }

    if(s.poisonMistEffect > 0)
    {
        for(auto &e:enemies)
        {
            float dist = Vector2Distance(e.pos,s.effectPos);

            if(dist < 130)
            {
                e.hp -= 7 * dt;
            }
        }
    }

    // BLACK HOLE
    if(s.blackHoleTimer > 5.0f)
    {
        Enemy* target = FindClosestEnemy(pl.pos,enemies,350);

        if(target)
        {
            s.effectPos = target->pos;
            s.blackHoleEffect = 2.5f;
        }

        s.blackHoleTimer = 0;
    }

    if(s.blackHoleEffect > 0)
    {
        for(auto &e:enemies)
        {
            Vector2 dir = Vector2Subtract(s.effectPos,e.pos);

            float dist = Vector2Length(dir);

            if(dist < 220)
            {
                dir = Vector2Normalize(dir);

                e.pos = Vector2Add(e.pos,Vector2Scale(dir,80*dt));

                if(dist < 40)
                {
                    e.hp -= 18 * dt;
                }
            }
        }
    }

    // BLOOD AURA
    if(s.bloodAuraTimer > 2.0f)
    {
        s.bloodAuraEffect = 0.5f;

        for(auto &e:enemies)
        {
            float dist = Vector2Distance(pl.pos,e.pos);

            if(dist < 140)
            {
                e.hp -=18 * dt;

                pl.hp += 1;

                if(pl.hp > pl.hpMax)
                    pl.hp = pl.hpMax;
            }
        }

        s.bloodAuraTimer = 0;
    }

    // SHOCKWAVE
    if(s.shockwaveTimer > 2.0f)
    {
        s.shockwaveEffect = 0.5f;

        for(auto &e:enemies)
        {
            float dist = Vector2Distance(pl.pos,e.pos);

            if(dist < 180)
            {
                Vector2 dir = Vector2Subtract(e.pos,pl.pos);

                dir = Vector2Normalize(dir);

                e.pos = Vector2Add(e.pos,Vector2Scale(dir,70));

                e.freezeTimer = 1.0f;

                e.hp -= 20;
            }
        }

        s.shockwaveTimer = 0;
    }

    // ลดเวลา effect
    if(s.voidMeteorEffect>0) s.voidMeteorEffect -= dt;
    if(s.chainLightningEffect>0) s.chainLightningEffect -= dt;
    if(s.poisonMistEffect>0) s.poisonMistEffect -= dt;
    if(s.blackHoleEffect>0) s.blackHoleEffect -= dt;
    if(s.bloodAuraEffect>0) s.bloodAuraEffect -= dt;
    if(s.shockwaveEffect>0) s.shockwaveEffect -= dt;
}
// DrawSkillEffects
void DrawSkillEffects(SkillState &s,player &pl)
{

    // VOID METEOR
    if(s.voidMeteorEffect>0)
    {
        DrawCircle(s.effectPos.x,s.effectPos.y,100,Fade(ORANGE,0.5f));
        DrawCircleLines(s.effectPos.x,s.effectPos.y,100,RED);
    }
    // PHANTOM BLADES
    if(s.phantomBladeTimer >= 0)
{
    float radius = 70;

    for(int i=0;i<3;i++)
    {
        float angle = s.phantomBladeAngle + i*2*PI/3;

        Vector2 pos =
        {
            pl.pos.x + cosf(angle)*radius,
            pl.pos.y + sinf(angle)*radius
        };

        DrawRectanglePro(
            {pos.x,pos.y,20,6},
            {10,3},
            angle*RAD2DEG,
            WHITE
        );
    }
}
    // CHAIN LIGHTNING
    if(s.chainLightningEffect>0)
    {
        for(size_t i=0;i+1<lightningPoints.size();i+=2)
        {
            DrawLineEx(
                lightningPoints[i],
                lightningPoints[i+1],
                3,
                YELLOW
            );
        }
    }
    // POISON MIST
    if(s.poisonMistEffect>0)
    {
        DrawCircle(s.effectPos.x,s.effectPos.y,130,Fade(GREEN,0.3f));
    }
    // BLACK HOLE
    if(s.blackHoleEffect>0)
    {
        DrawCircleLines(s.effectPos.x,s.effectPos.y,220,PURPLE);
        DrawCircle(s.effectPos.x,s.effectPos.y,40,Fade(PURPLE,0.6f));
    }
    // BLOOD AURA
    if(s.bloodAuraEffect>0)
    {
        DrawCircle(pl.pos.x,pl.pos.y,140,Fade(RED,0.3f));
        DrawCircleLines(pl.pos.x,pl.pos.y,140,RED);
    }
    // SHOCKWAVE
    if(s.shockwaveEffect>0)
    {
        DrawCircleLines(pl.pos.x,pl.pos.y,180,SKYBLUE);
    }

}