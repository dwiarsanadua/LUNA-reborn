--[[
  Quest 168: Orcs Alliance - Lamias
  Level Required: 38
  NPC Start: 33, NPC Complete: 33
  Prerequisites: [167]
  Rewards: EXP=102067, Gold=5025
]]

local fsm = require('fsm_engine')

local quest_168 = fsm:new({
    id = 168,
    name = "Orcs Alliance - Lamias",
    level_required = 38,
    npc_start = 33,
    npc_complete = 33,
    prerequisites = {167},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 168: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 168: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 168: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 32,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 168: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 168: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 168: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 168: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 168: Rewards given")
                self:give_rewards({
                    exp = 102067,
                    gold = 5025,
                    items = {
                        { item_id = 83, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_168