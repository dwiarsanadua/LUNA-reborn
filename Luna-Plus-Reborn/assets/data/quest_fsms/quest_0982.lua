--[[
  Quest 982: [Dungeon Quest]The Temple of Greed (Intermediate)
  Level Required: 90
  NPC Start: 64, NPC Complete: 64
  Rewards: EXP=5222395, Gold=61188
]]

local fsm = require('fsm_engine')

local quest_982 = fsm:new({
    id = 982,
    name = "[Dungeon Quest]The Temple of Greed (Intermediate)",
    level_required = 90,
    npc_start = 64,
    npc_complete = 64,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 982: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 982: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 982: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 649,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 982: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 657,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 982: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 658,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 982: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 659,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 982: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 982: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 982: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 982: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 982: Rewards given")
                self:give_rewards({
                    exp = 5222395,
                    gold = 61188,
                })
            end,
            transitions = {},
        },
    },
})

return quest_982