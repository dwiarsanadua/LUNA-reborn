--[[
  Quest 835: [Special] The Queen of the Mark
  Level Required: 79
  NPC Start: 97, NPC Complete: 97
  Prerequisites: [416]
  Rewards: EXP=2627089, Gold=61514
]]

local fsm = require('fsm_engine')

local quest_835 = fsm:new({
    id = 835,
    name = "[Special] The Queen of the Mark",
    level_required = 79,
    npc_start = 97,
    npc_complete = 97,
    prerequisites = {416},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 835: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 835: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 835: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 267,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 835: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 835: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 835: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 835: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 835: Rewards given")
                self:give_rewards({
                    exp = 2627089,
                    gold = 61514,
                    items = {
                        { item_id = 393, count = 1 },
                        { item_id = 21000513, count = 5 },
                        { item_id = 21000514, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_835