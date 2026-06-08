--[[
  Quest 897: A Dazzling Introduction
  Level Required: 98
  NPC Start: 541, NPC Complete: 541
  Rewards: EXP=8938143, Gold=77721
]]

local fsm = require('fsm_engine')

local quest_897 = fsm:new({
    id = 897,
    name = "A Dazzling Introduction",
    level_required = 98,
    npc_start = 541,
    npc_complete = 541,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 897: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 897: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 897: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 330,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 897: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 331,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 897: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 897: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 897: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 897: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 897: Rewards given")
                self:give_rewards({
                    exp = 8938143,
                    gold = 77721,
                })
            end,
            transitions = {},
        },
    },
})

return quest_897