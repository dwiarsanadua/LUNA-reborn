--[[
  Quest 2012: [Daily] Tarantula Subjugation
  Level Required: 62
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=300977
]]

local fsm = require('fsm_engine')

local quest_2012 = fsm:new({
    id = 2012,
    name = "[Daily] Tarantula Subjugation",
    level_required = 62,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2012: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2012: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2012: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 249,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2012: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2012: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2012: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2012: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2012: Rewards given")
                self:give_rewards({
                    exp = 300977,
                    items = {
                        { item_id = 30000835, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2012