--[[
  Quest 806: Dark Portal
  Level Required: 69
  NPC Start: 39, NPC Complete: 39
  Rewards: EXP=1073249, Gold=21542
]]

local fsm = require('fsm_engine')

local quest_806 = fsm:new({
    id = 806,
    name = "Dark Portal",
    level_required = 69,
    npc_start = 39,
    npc_complete = 39,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 806: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 39,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 806: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 806: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 641,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 806: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 39,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 806: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 806: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 39,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 806: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 806: Rewards given")
                self:give_rewards({
                    exp = 1073249,
                    gold = 21542,
                    items = {
                        { item_id = 21000513, count = 5 },
                        { item_id = 21000514, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_806