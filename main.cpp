
#include "classes.h"



sf::Mutex mutex; //required so that draw() and genrate_pariticel() dont fuck up with existing paritcels
// class Falling_Enemy;


 

 //  PLAYER::keyboardInput
  void PLAYER::keyboardInput(Falling_Enemy& Enemy_Set, sf::RenderWindow &window){
      sf::Vector2f currentPosition = (this->circle).getPosition();
      float PlayerMovSpeed = 0.3;
     
      if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
        if(currentPosition.y-PlayerMovSpeed < 0)
          currentPosition.y=0;
        else 
          currentPosition.y-=PlayerMovSpeed;
        (this->circle).setPosition(currentPosition);
      }

      if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
        if(currentPosition.y+PlayerMovSpeed+2*(this->circle).getRadius() >= (this->WindDims).y)
          currentPosition.y = (this->WindDims).y-2*(this->circle).getRadius();
        else
          currentPosition.y+=PlayerMovSpeed;
        (this->circle).setPosition(currentPosition);
      }

      if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
        if(currentPosition.x-PlayerMovSpeed < 0)
          currentPosition.x=0;
        else
          currentPosition.x-=PlayerMovSpeed;
        (this->circle).setPosition(currentPosition);
      }
      
      if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
        if(currentPosition.x+PlayerMovSpeed+2*(this->circle).getRadius() > (this->WindDims).x)
          currentPosition.x = (this->WindDims).x- 2*(this->circle).getRadius();
        else
          currentPosition.x+=PlayerMovSpeed;
        (this->circle).setPosition(currentPosition);
      }
      

      if(sf::Keyboard::isKeyPressed(sf::Keyboard::P)){
        this->createAttackParticle();
      }


      if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
        std::cout<<"Paused\n";
        this->PauseGame(Enemy_Set,window);
      }

      
    }

//  PLAYER::PauseGame
  void PLAYER::PauseGame(Falling_Enemy& Enemy_Set, sf::RenderWindow &window){
    this->GLOBAL_PAUSE = true;
    while(1){
        
        sf::Font font;
        font.loadFromFile("Arial.ttf");
        sf::Text pauseMessage;
        pauseMessage.setFont(font);
        pauseMessage.setFillColor(sf::Color(0,255,0));
        pauseMessage.setCharacterSize(19);
        pauseMessage.setStyle(sf::Text::Italic);
        pauseMessage.setPosition(window.getSize().x/2, window.getSize().y/2);
        pauseMessage.setString(sf::String("Paused \n Press C to continue"));
        window.clear();
        window.draw(pauseMessage);
        window.display();

        // Unpause game
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::C)){
          Enemy_Set.resetClock();
          this->GLOBAL_PAUSE = 0;
          break;
        }
    

        
      }



    this->GLOBAL_PAUSE = false;
  }



int main(){
  srand(std::time(0));
  srand(0);
  int width_WIN = 800, height_WIN = 600;  
  sf::RenderWindow window(sf::VideoMode(width_WIN,height_WIN),"game" );

  float circleRadius=20.f;



  PLAYER player1(window,circleRadius);
  Falling_Enemy Enemy(30,window.getSize());

  


  // STARTING THREADS
  // sf::Thread TH_Keyboard(std::bind(&PLAYER::Keyboard_thread, &player1,std::ref(window)));  // bind calls copy contrutor
  // TH_Keyboard.launch();

  
  while(window.isOpen()){
    sf::Event event;
    // player1.keyboardInput();
    while(window.pollEvent(event)){
      if(event.type == sf::Event::Closed){
        window.close();
        // TH_Keyboard.terminate();
        
      }
    }


    // Player management
    player1.keyboardInput(Enemy,window);
    


  window.clear();

  // mutex.lock();
  Enemy.manageEnemyAndCollisions(player1);
  player1.managePlayerAttacks();


  window.draw(player1);
  window.draw(Enemy);
  // mutex.unlock();
  


/*why mutesx 
because thread TH_Keyboard and this main function share common reource player1.particle array
*/

  std::cout<<"Current Particels "<<player1.getPlayerAttacks().size()<<std::endl;
  std::cout<<"Current Enemies "  <<Enemy.getEnemy().size()<<std::endl;

  window.display();

  }
  
  
}