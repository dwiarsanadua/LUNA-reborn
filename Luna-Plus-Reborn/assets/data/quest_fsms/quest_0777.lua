--[[
  Quest 777: Gift of Equus
  Level Required: 0
  NPC Start: 29, NPC Complete: 29
  Prerequisites: [774, 775, 776]
  Rewards: EXP=53421, Gold=2901
]]

local fsm = require('fsm_engine')

local quest_777 = fsm:new({
    id = 777,
    name = "Gift of Equus",
    level_required = 0,
    npc_start = 29,
    npc_complete = 29,
    prerequisites = {774, 775, 776},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 777: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 777: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 777: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 777: NPC talk objective met")
                    end
                },
                {
                    trigger = "any",
                    target = "COMPLETE",
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 777: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 777: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 777: Rewards given")
                self:give_rewards({
                    exp = 53421,
                    gold = 2901,
                    items = {
                        { item_id = 21000282, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_777