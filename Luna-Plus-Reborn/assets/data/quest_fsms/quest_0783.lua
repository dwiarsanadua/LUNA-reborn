--[[
  Quest 783: Food Protection
  Level Required: 45
  NPC Start: 62, NPC Complete: 62
  Rewards: EXP=117652, Gold=7415
]]

local fsm = require('fsm_engine')

local quest_783 = fsm:new({
    id = 783,
    name = "Food Protection",
    level_required = 45,
    npc_start = 62,
    npc_complete = 62,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 783: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 783: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 783: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 77,
                    count = 25,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 783: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 783: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 783: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 783: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 783: Rewards given")
                self:give_rewards({
                    exp = 117652,
                    gold = 7415,
                    items = {
                        { item_id = 30000685, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_783