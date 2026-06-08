--[[
  Quest 461: [Change of Job] Way of Blade Taker
  Level Required: 105
  NPC Start: 11, NPC Complete: 122
  Prerequisites: [453]
]]

local fsm = require('fsm_engine')

local quest_461 = fsm:new({
    id = 461,
    name = "[Change of Job] Way of Blade Taker",
    level_required = 105,
    npc_start = 11,
    npc_complete = 122,
    prerequisites = {453},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 461: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 461: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 461: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 461: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 461: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 461: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 461: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 461: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 30000544, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_461