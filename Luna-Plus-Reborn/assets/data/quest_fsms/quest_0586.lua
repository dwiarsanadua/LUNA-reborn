--[[
  Quest 586: [Quest] For Costumes!
  Level Required: 1
  NPC Start: 300, NPC Complete: 300
]]

local fsm = require('fsm_engine')

local quest_586 = fsm:new({
    id = 586,
    name = "[Quest] For Costumes!",
    level_required = 1,
    npc_start = 300,
    npc_complete = 300,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 586: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 300,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 586: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 586: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 300,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 586: NPC talk objective met")
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
                self:log("Quest 586: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 300,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 586: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 586: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 21001097, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_586