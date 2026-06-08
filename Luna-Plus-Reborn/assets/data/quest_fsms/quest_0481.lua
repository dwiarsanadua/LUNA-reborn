--[[
  Quest 481: [Special][Quest] Sage's Gift
  Level Required: 105
  NPC Start: 122, NPC Complete: 122
]]

local fsm = require('fsm_engine')

local quest_481 = fsm:new({
    id = 481,
    name = "[Special][Quest] Sage's Gift",
    level_required = 105,
    npc_start = 122,
    npc_complete = 122,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 481: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 481: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 481: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 481: NPC talk objective met")
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
                self:log("Quest 481: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 481: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 481: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 13002073, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_481