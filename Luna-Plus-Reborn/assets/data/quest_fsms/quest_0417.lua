--[[
  Quest 417: Goblin Invaders
  Level Required: 19
  NPC Start: 17, NPC Complete: 17
  Rewards: EXP=5735, Gold=2000
]]

local fsm = require('fsm_engine')

local quest_417 = fsm:new({
    id = 417,
    name = "Goblin Invaders",
    level_required = 19,
    npc_start = 17,
    npc_complete = 17,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 417: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 17,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 417: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 417: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 6,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 417: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 49,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 417: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 48,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 417: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 17,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 417: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 417: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 17,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 417: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 417: Rewards given")
                self:give_rewards({
                    exp = 5735,
                    gold = 2000,
                    items = {
                        { item_id = 21000562, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_417