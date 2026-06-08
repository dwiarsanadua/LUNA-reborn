--[[
  Quest 758: [Quest] First Battle
  Level Required: 0
  NPC Start: 402, NPC Complete: 407
  Prerequisites: [757]
  Rewards: EXP=100000, Gold=10000
]]

local fsm = require('fsm_engine')

local quest_758 = fsm:new({
    id = 758,
    name = "[Quest] First Battle",
    level_required = 0,
    npc_start = 402,
    npc_complete = 407,
    prerequisites = {757},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 758: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 402,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 758: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 758: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 55,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 758: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 402,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 758: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 407,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 758: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 758: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 407,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 758: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 758: Rewards given")
                self:give_rewards({
                    exp = 100000,
                    gold = 10000,
                    items = {
                        { item_id = 13000124, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_758