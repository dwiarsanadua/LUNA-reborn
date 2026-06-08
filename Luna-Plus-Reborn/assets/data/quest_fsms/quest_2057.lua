--[[
  Quest 2057: [Daily] Support Delivery 1
  Level Required: 107
  NPC Start: 412, NPC Complete: 412
  Rewards: EXP=3462111
]]

local fsm = require('fsm_engine')

local quest_2057 = fsm:new({
    id = 2057,
    name = "[Daily] Support Delivery 1",
    level_required = 107,
    npc_start = 412,
    npc_complete = 412,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2057: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2057: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2057: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2057: NPC talk objective met")
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
                self:log("Quest 2057: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2057: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2057: Rewards given")
                self:give_rewards({
                    exp = 3462111,
                    items = {
                        { item_id = 30000836, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2057