--[[
  Quest 2045: [Daily] Support Nera Harbor 1
  Level Required: 95
  NPC Start: 412, NPC Complete: 412
  Rewards: EXP=3511398
]]

local fsm = require('fsm_engine')

local quest_2045 = fsm:new({
    id = 2045,
    name = "[Daily] Support Nera Harbor 1",
    level_required = 95,
    npc_start = 412,
    npc_complete = 412,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2045: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2045: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2045: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 290,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2045: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2045: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2045: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2045: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2045: Rewards given")
                self:give_rewards({
                    exp = 3511398,
                    items = {
                        { item_id = 30000836, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2045