#include<SFML/Graphics.hpp>
#include<SFML/System/Time.hpp>

#include<iostream>
#include<vector>
#include<cmath>
#include<functional>
#include<unordered_map>
class Falling_Enemy;


class ParticleSystem : public sf::Drawable , public  sf::Transformable {
private:
    struct particle{
            sf::Vector2f velocity;
            sf::Time lifetime ;
        };
    sf::Vector2f m_emitter;
    std::vector<particle> m_particles;
    sf::VertexArray m_positions;
    sf::Clock EffectDisplClk;
    sf::Clock NewCollision;
    float DefaultLife;

  
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const{
        // apply the transform
        states.transform *= getTransform();

        // our particles don't use a texture
        states.texture = NULL;

        // draw the vertex array
        target.draw(m_positions, states);
    }


public:
   void Set_ParticleCount(int particleCount){
        DefaultLife = 10;
        m_particles.resize(particleCount);
        m_positions.setPrimitiveType(sf::Points) ;
        m_positions.resize(particleCount);
        m_emitter = {0.f,0.f};
        EffectDisplClk.restart();

    
        
        }



    float distBWPoints(sf::Vector2f a, sf::Vector2f b){
      float A = std::pow(a.x - b.x,2);
      float B = std::pow(a.y - b.y,2);

      float dist = std::sqrt(A+B);

      return dist;
    }



    void update(){
        sf::Time elapsed = EffectDisplClk.getElapsedTime();
        for(std::size_t i=0; i<m_particles.size(); i++){
            particle &p = m_particles[i];

            p.lifetime-= elapsed;


            if(p.lifetime<= sf::Time::Zero)
                reset_particle(i);
            // if(p.lifetime<= sf::Time::Zero)

            // postion updater
            m_positions[i].position += elapsed.asSeconds() * p.velocity;


            // trasnapercney updater

            float dist = distBWPoints(m_emitter,m_positions[i].position );  // distance between partielce and emitter
            float ratio = DefaultLife/dist;
            m_positions[i].color.a = static_cast<sf::Uint8>(ratio * 255);


            if(  NewCollision.getElapsedTime().asSeconds()>=1){
              m_positions[i].color.a = 0;
              m_positions[i].position = {10000,10000};
            }
        }
        EffectDisplClk.restart();
    }


    void reset_particle(int index){
        // reseting velocity
        float speed = std::rand() % 50 + 50.f ;
        float angle = (std::rand()%360)* 3.14f / 180.f;  // degree to radian
        m_particles[index].velocity = sf::Vector2f(std::cos(angle)*speed, std::sin(angle)*speed);

        m_particles[index].lifetime = sf::milliseconds(std::rand()%2000 + 1000);
        m_positions[index].position = m_emitter;


        // std::cout<<"resrintg parficles\n";
    }



    void change_emitter(sf::Vector2f org){
        NewCollision.restart();
        m_emitter = org;
    }
};



void printVector2f(sf::Vector2f vec){
  std::cout<<"X: "<<vec.x<<", Y: "<<vec.y<<std::endl;
}
bool debug = true;


struct ATTACK_PARTCLE{
  sf::CircleShape particle;
  sf::Vector2f Velocity;
  bool UsedUp = false;
};
class PLAYER : public sf::Transformable, public sf::Drawable{
  friend class Falling_Enemy; 
  private:
    int SCORE{};
    bool GLOBAL_PAUSE= false;
    sf::CircleShape circle;
    std::vector<ATTACK_PARTCLE> playerParticles;
    sf::Vector2u WindDims;
    sf::Vector2f playerVelocity;
    sf::Clock PartSpdClk;
    sf::Clock PartSepClk;  
    sf::Text Score_Text;
    sf::Font font;

    virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const{// const can be called for const obj                                             
      states.transform *= getTransform(); // getTransform() is defined by sf::Transformable
      /*QUICK LESSON
       states.transform -> current transofrmation of object
      */
      
      // states.texture =  texture ;

    // sf::CircleShape sh;
    // sh.setPosition(sf::Vector2f(50,201));
    // sh.setRadius(77.0f);

    //   target.draw(sh,states);

      target.draw(circle,states);
      for(ATTACK_PARTCLE p: ( this->playerParticles))
        target.draw(p.particle);
      target.draw(Score_Text,states);
    }

  void KillPlayer(){
    // pariticle effect
    SCORE = 0;
    circle.setPosition(sf::Vector2f(0, WindDims.y- 2*circle.getRadius() ));
    circle.setFillColor(sf::Color(255,50,30));

  }


  void PauseGame(Falling_Enemy& Enemy_Set, sf::RenderWindow &window);

  public:
    PLAYER(sf::RenderWindow & window, int radius){
      circle.setRadius(radius);
      
      this->WindDims =  window.getSize();
      circle.setPosition(sf::Vector2f(0, window.getSize().y- 2*radius ));
      circle.setFillColor(sf::Color(255,50,30));

      this->playerVelocity = sf::Vector2f(30,0); // speed, angle



      // intializing Fonts and texts
      
      font.loadFromFile("Arial.ttf");
      Score_Text.setFont(font);
      Score_Text.setStyle(sf::Text::Bold);
      Score_Text.setCharacterSize(24);
      Score_Text.setPosition(sf::Vector2f(WindDims.y-30,0));
      Score_Text.setString("Score  " +std::to_string (getScore()));

    }

    std::vector<ATTACK_PARTCLE>   getPlayerAttacks(){
      return (this->playerParticles);
     }
   
   
  
    void keyboardInput(Falling_Enemy &Enemy_Set, sf::RenderWindow &window);



    bool getPauseStatus(){
      return GLOBAL_PAUSE;
    }

    void createAttackParticle(){
      int PARTICLE_SEP_TIME= 100;
      int PARTICLE_ATTACK_SPEED= 1000;
      int PARTICLE_MAX_LIMIT = 1000000;
      float elapsedTime = (this->PartSepClk).getElapsedTime().asMilliseconds();
      if(elapsedTime <= PARTICLE_SEP_TIME )return;

      
      if((this->playerParticles).size() > PARTICLE_MAX_LIMIT ) return ;
      
  
      ATTACK_PARTCLE p ;
      p.particle.setRadius(10.0f);
      p.particle.setPosition(sf::Vector2f((this->circle).getPosition().x + (this->circle).getRadius()-10 ,
                              (this->circle).getPosition().y));

      sf::Uint32 R( std::rand()%255 ),  G( std::rand()%255 ), B( std::rand()%255 );

      p.particle.setFillColor(sf::Color(R,G,B));
      p.Velocity= sf::Vector2f(PARTICLE_ATTACK_SPEED,90);

      (this->playerParticles).push_back(p);
      


      (this->PartSepClk).restart();
    }


    void UpdateScore(){
 
    Score_Text.setString("Score  " +std::to_string (getScore()));
    }



    void managePlayerAttacks(){
      float elapsedTime =   (this->PartSpdClk).getElapsedTime().asSeconds();
   
      
      for(int i=0; i< (this->playerParticles).size(); i++){
        if(playerParticles[i].UsedUp){
          playerParticles.erase(playerParticles.begin() + i);
          std::cout<<"eraseing platicel\n";
          continue;
        }

        float delta_x = playerParticles[i].Velocity.x *
            std::cos( playerParticles[i].Velocity.y * (3.14 / 180.0) );

        float delta_y = playerParticles[i].Velocity.x * 
            std::sin( playerParticles[i].Velocity.y * (3.14 / 180.0) );



        float new_x = playerParticles[i].particle.getPosition().x   + delta_x*elapsedTime ;
        float new_y = playerParticles[i].particle.getPosition().y   - delta_y*elapsedTime  ;
        

        if(new_x<0 || new_x >=(this->WindDims).x || new_y <0  || new_y>= (this->WindDims).y)
          playerParticles.erase(playerParticles.begin()+i);


        else{ 
          playerParticles[i].particle.setPosition(sf::Vector2f(new_x,new_y));
          sf::Color colr=  playerParticles[i].particle.getFillColor();
          float displRatio = playerParticles[i].particle.getPosition().y  / ((this)->WindDims).y  ;
          if(displRatio <= 0.4)displRatio = 0.5;
          colr.a =   displRatio* 255 ;
          playerParticles[i].particle.setFillColor(colr);
        }
      }
    (PartSpdClk).restart();

    UpdateScore();
  } 


  int getScore(){
    return SCORE;
  }


};







struct ENEMY_PARTCLE{
  sf::CircleShape entity;
  sf::Vector2f Velocity;  
  bool Collided = false;
};

class Falling_Enemy: public sf::Transformable  , public sf::Drawable{
  
private:
  std::vector<ENEMY_PARTCLE> Enemy_Array;
  sf::Vector2u WinDims;
  int enemyBase{};
  sf::Clock EnmSepClock;
  sf::Clock EnmSpeedClock;
  ParticleSystem ParticleEffect;
  int ParticleEffectCount;
  

  virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const{
    states.transform*= getTransform();
    

    target.draw(ParticleEffect);
    for(auto i:Enemy_Array)
      target.draw(i.entity,states);
  }


public:
  
  Falling_Enemy(int enemyBase, sf::Vector2u WinDims){
    this->ParticleEffectCount= 50;
    ParticleEffect.Set_ParticleCount(ParticleEffectCount);
    
    this->enemyBase = enemyBase;
    this->WinDims = WinDims;
    EnmSepClock.restart();
    EnmSpeedClock.restart();
    // for(int i=0; i<10; i++)
    //   spawnEnemy();
    // std::cout<<"Spawnign enemy  " << Enemy_Array.size() ;
    
  }

  std::vector<ENEMY_PARTCLE> getEnemy(){
    return Enemy_Array;
  }


  void spawnEnemy(){
        int ENEMY_SEP_TIME= 300;
    int ENEMY_ATTACK_SPEED= 90;
    int ENEMY_MAX_LIMIT = 1000000;
    float elapsedTime = (this->EnmSepClock).getElapsedTime().asMilliseconds();
    if(elapsedTime <= ENEMY_SEP_TIME )return;

    std::cout<<"spawning enemy\n";
    
    if((this->Enemy_Array).size() > ENEMY_MAX_LIMIT ) return ;
    

    ENEMY_PARTCLE e;
    e.entity.setRadius(enemyBase);
    int org = std::rand() % (this->WinDims.x - enemyBase);
    if(org+enemyBase*2 > WinDims.x)org = WinDims.x- 2*enemyBase; 
    e.entity.setPosition(sf::Vector2f(org,0));
    e.entity.setPointCount(3);
    e.entity.setFillColor(sf::Color(std::rand()%255 ,std::rand()%255,std::rand()%255 ));
   
    e.Velocity= sf::Vector2f(ENEMY_ATTACK_SPEED,270);

    (this->Enemy_Array).push_back(e);
    
    (this->EnmSepClock).restart();
  }

  void CleanEnemy(){
    for(int i=0; i<Enemy_Array.size(); i++){
      if(Enemy_Array[i].Collided){
        Enemy_Array.erase(Enemy_Array.begin()+ i);
      }
    }

  }

  void manageEnemyAndCollisions(PLAYER &player){
    spawnEnemy();
    float elapsedTime =   (this->EnmSpeedClock).getElapsedTime().asSeconds();


    for(int i=0; i< (this->Enemy_Array).size(); i++){
      float delta_x = Enemy_Array[i].Velocity.x *
          std::cos( Enemy_Array[i].Velocity.y * (3.14 / 180.0) );

      float delta_y =Enemy_Array[i].Velocity.x * 
          std::sin( Enemy_Array[i].Velocity.y * (3.14 / 180.0) );



      float new_x = Enemy_Array[i].entity.getPosition().x   + delta_x*elapsedTime ;
      float new_y = Enemy_Array[i].entity.getPosition().y   - delta_y*elapsedTime  ;
      

      if(new_x<0 || new_x >=(this->WinDims).x || new_y <0  || new_y>= (this->WinDims).y){
        Enemy_Array.erase(Enemy_Array.begin()+i);
        player.SCORE--;
      }


      else{ 
        Enemy_Array[i].entity.setPosition(sf::Vector2f(new_x,new_y));
      }
    }
  (EnmSpeedClock).restart();

  CheckCollisions(player);

  } 

  void resetClock(){
    EnmSepClock.restart();
    EnmSpeedClock.restart();
  }
// CHECKS COLLISION UPDATES SCORE KLLS PLAYER
  void CheckCollisions(PLAYER &player){
    bool IFCOLLISION= false;
    std::vector<ATTACK_PARTCLE>attackParticle = player.getPlayerAttacks();
      // collision detection
    for(int i=0; i<Enemy_Array.size();i++){
      //check for player collision
      sf::FloatRect bndbxEnemy, bndbxPlayer;
      bndbxEnemy = Enemy_Array[i].entity.getGlobalBounds();
      bndbxPlayer = player.circle.getGlobalBounds();
      if(bndbxEnemy.intersects(bndbxPlayer)){
        player.KillPlayer();            //Killing player

        //reseting particles
        for(auto &i:Enemy_Array)
          i.Collided=true;
        break;
      }


      for(int j=0; j<(attackParticle).size(); j++){
        if(attackParticle[j].UsedUp)continue;  //If particel is used then continue;
        sf::FloatRect bndbxEnemy, bndbxParticle;
        bndbxEnemy = Enemy_Array[i].entity.getGlobalBounds();
        bndbxParticle = (attackParticle)[j].particle.getGlobalBounds();
        
        if(bndbxEnemy.contains(bndbxParticle.getPosition())){
          IFCOLLISION = true;
          std::cout<<"Hell Yeah Collision time for particles\n";
          (player.SCORE)++;
          Enemy_Array[i].Collided = true;  
          player.playerParticles[j].UsedUp = true;
          ParticleEffect.change_emitter(Enemy_Array[i].entity.getPosition());
        }
      }
      
    }


    // if (!IFCOLLISION)ParticleEffect.change_emitter(sf::Vector2f(0,0));
    ParticleEffect.update();
    this->CleanEnemy();

      
  }


  


};
