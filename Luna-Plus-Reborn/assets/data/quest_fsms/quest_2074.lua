--[[
  Quest 2074: [Daily] The Temple of Greed(Intermediate)
  Level Required: 90
  NPC Start: 412, NPC Complete: 412
  Rewards: EXP=7177154
]]

local fsm = require('fsm_engine')

local quest_2074 = fsm:new({
    id = 2074,
    name = "[Daily] The Temple of Greed(Intermediate)",
    level_required = 90,
    npc_start = 412,
    npc_complete = 412,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2074: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2074: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2074: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 647,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2074: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2074: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2074: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2074: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2074: Rewards given")
                self:give_rewards({
                    exp = 7177154,
                    items = {
                        { item_id = 30000836, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2074