--[[
  Quest 992: Quest 992
  Level Required: 150
  NPC Start: 9, NPC Complete: 9
]]

local fsm = require('fsm_engine')

local quest_992 = fsm:new({
    id = 992,
    name = "Quest 992",
    level_required = 150,
    npc_start = 9,
    npc_complete = 9,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 992: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 992: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 992: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 992: NPC talk objective met")
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
                self:log("Quest 992: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 992: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 992: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 21001719, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_992