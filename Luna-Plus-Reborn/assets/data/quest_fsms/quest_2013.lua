--[[
  Quest 2013: [Daily] Shadow Subjugation
  Level Required: 63
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=479152
]]

local fsm = require('fsm_engine')

local quest_2013 = fsm:new({
    id = 2013,
    name = "[Daily] Shadow Subjugation",
    level_required = 63,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2013: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2013: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2013: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 16,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2013: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2013: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2013: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2013: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2013: Rewards given")
                self:give_rewards({
                    exp = 479152,
                    items = {
                        { item_id = 30000835, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2013